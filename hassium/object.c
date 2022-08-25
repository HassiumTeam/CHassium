#include <object.h>

static struct obj *toString(struct obj *, struct vm *, struct vec *);

static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);

static struct builtin_ops object_builtin_ops = {
    .__iter__ = __iter__,
};

struct obj *obj_new(obj_ctx_type_t type, void *ctx, struct obj *obj_type) {
  struct obj *obj = malloc(sizeof(struct obj));
  obj->refs = 0;
  obj->ref_immune = false;
  obj->type = type;
  obj->ctx = ctx;
  obj->obj_type = obj_inc_ref(obj_type);
  obj->ops = &object_builtin_ops;
  obj->parent = &object_type_obj;
  obj->attribs = obj_hashmap_new();
  obj->lazy_load_fn = NULL;
  obj->weak_refs = NULL;

  return obj;
}

static struct obj *__iter__(struct obj *obj, struct vm *vm, struct vec *args) {
  struct obj *keys_argstack[1];
  keys_argstack[0] = obj;
  struct vec keys_args;
  keys_args.data = (void **)keys_argstack;
  keys_args.len = 1;

  struct obj *keys = object_keys(NULL, vm, &keys_args);
  keys->parent = obj_inc_ref(obj);
  struct obj *iter = obj_iter_new(keys);

  obj_set_attrib(iter, "__iterfull__",
                 obj_builtin_new(__iter____iterfull__, iter));
  obj_set_attrib(iter, "__iternext__",
                 obj_builtin_new(__iter____iternext__, iter));

  return iter;
}

static struct obj *__iter____iterfull__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  struct vec *arr = iter->target->ctx;

  return bool_to_obj(iter->pos >= arr->len);
}

static struct obj *__iter____iternext__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  struct vec *arr = iter->target->ctx;

  struct obj *key = vec_get(arr, iter->pos++);
  struct obj *ret = obj_new(OBJ_ANON, NULL, &object_type_obj);
  obj_set_attrib(ret, "key", key);
  obj_set_attrib(
      ret, "val",
      obj_hashmap_get(iter->target->parent->attribs, (char *)key->ctx));

  return ret;
}

void obj_free(struct obj *obj) {
  if (obj->weak_refs != NULL) {
    struct obj **ref;
    for (int i = 0; i < obj->weak_refs->len; i++) {
      ref = vec_get(obj->weak_refs, i);
      *ref = &none_obj;
    }
    vec_free(obj->weak_refs);
  }

  switch (obj->type) {
    case OBJ_ARRAY: {
      struct vec *items = obj->ctx;
      for (int i = 0; i < items->len; i++) {
        obj_dec_ref(vec_get(items, i));
      }
      vec_free(items);
    } break;
    case OBJ_BUILTIN: {
      struct builtin_obj_ctx *builtin = obj->ctx;
      obj_dec_ref(builtin->self);
      free(builtin);
    } break;
    case OBJ_FUNC: {
      struct func_obj_ctx *func = obj->ctx;
      obj_dec_ref(func->self);
      stackframe_dec_ref(func->frame);
      free(func);
    } break;
    case OBJ_ITER: {
      struct iter_obj_ctx *iter = obj->ctx;
      obj_dec_ref(iter->target);
      free(iter);
    } break;
    case OBJ_NUM: {
    } break;
    case OBJ_WEAKREF:
      if (obj->ctx != &none_obj) {
        struct obj *ref = obj->ctx;
        vec_remove(ref->weak_refs, &obj->ctx);
      }
      break;
    case OBJ_TYPE:
      break;
    default:
      if (obj->ctx != NULL) free(obj->ctx);
      break;
  }

  if (obj->attribs != NULL) obj_hashmap_free(obj->attribs);
  if (obj->parent != NULL) obj_dec_ref(obj->parent);
  if (obj->obj_type != NULL) obj_dec_ref(obj->obj_type);

  free(obj);
}

struct obj *obj_bin_op(bin_op_type_t type, struct obj *left, struct obj *right,
                       struct vm *vm) {
  struct obj *argstack[1];
  struct vec args;
  args.data = (void **)argstack;
  args.len = 1;
  args.data[0] = right;

  switch (type) {
    case BIN_OP_ADD:
      if (left->ops && left->ops->__add__) {
        return left->ops->__add__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__add__", vm, &args);
      }
    case BIN_OP_AND:
      return bool_to_obj(obj_is_true(left, vm) && obj_is_true(right, vm));
    case BIN_OP_DIV:
      if (left->ops && left->ops->__div__) {
        return left->ops->__div__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__div__", vm, &args);
      }
    case BIN_OP_EQ:
      if (left == right) {
        return &true_obj;
      } else if (left->ops && left->ops->__eq__) {
        return left->ops->__eq__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__eq__", vm, &args);
      }
    case BIN_OP_GREATER:
      if (left->ops && left->ops->__greater__) {
        return left->ops->__greater__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__greater__", vm, &args);
      }
    case BIN_OP_GREATER_OR_EQ:
      if (obj_bin_op(BIN_OP_EQ, left, right, vm) == &true_obj) {
        return &true_obj;
      } else if (left->ops && left->ops->__greater__) {
        return left->ops->__greater__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__add__", vm, &args);
      }
    case BIN_OP_LESSER:
      if (left->ops && left->ops->__lesser__) {
        return left->ops->__lesser__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__lesser__", vm, &args);
      }
    case BIN_OP_LESSER_OR_EQ:
      if (obj_bin_op(BIN_OP_EQ, left, right, vm) == &true_obj) {
        return &true_obj;
      } else if (left->ops && left->ops->__lesser__) {
        return left->ops->__lesser__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__lesser__", vm, &args);
      }
    case BIN_OP_MOD:
      if (left->ops && left->ops->__mod__) {
        return left->ops->__mod__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__mod__", vm, &args);
      }
    case BIN_OP_MUL:
      if (left->ops && left->ops->__mul__) {
        return left->ops->__mul__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__mul__", vm, &args);
      }
    case BIN_OP_OR:
      return bool_to_obj(obj_is_true(left, vm) || obj_is_true(right, vm));
    case BIN_OP_SUB:
      if (left->ops && left->ops->__sub__) {
        return left->ops->__sub__(left, vm, &args);
      } else {
        return obj_invoke_attrib(left, "__sub__", vm, &args);
      }
  }
}

struct obj *obj_eq(struct obj *left, struct obj *right, struct vm *vm) {
  if (left->ops != NULL) {
    struct vec *args = vec_new();
    vec_push(args, right);
    struct obj *ret = left->ops->__eq__(left, vm, args);
    vec_free(args);
    return ret;
  } else if (obj_hashmap_has(left->attribs, "__eq__")) {
    struct vec *args = vec_new();
    vec_push(args, right);
    struct obj *ret = obj_invoke_attrib(left, "__eq__", vm, args);
    vec_free(args);
    return ret;
  } else {
    return bool_to_obj(left == right);
  }
}

struct obj *obj_index(struct obj *target, struct obj *key, struct vm *vm) {
  struct obj *index = obj_hashmap_get(target->attribs, "__index__");
  if (index != &none_obj) {
    struct obj *key_argsstack[1];
    key_argsstack[0] = key;
    struct vec key_args;
    key_args.data = (void **)key_argsstack;
    key_args.len = 1;
    return obj_invoke(index, vm, &key_args);
  } else {
    if (key->type != OBJ_STRING) {
      printf("Object must be accessed by a string!");
      exit(-1);
    }
    return obj_hashmap_get(target->attribs, obj_string_val(key));
  }
}

static void instantiate_attrib(void *key, size_t ksize, uintptr_t value,
                               void *usr) {
  struct obj *new = usr;
  struct obj *attrib_val = (struct obj *)value;

  if (attrib_val->type == OBJ_FUNC) {
    struct func_obj_ctx *func_ctx = attrib_val->ctx;
    // At one point there was an extra stackframe_inc_ref on the frame
    struct obj *new_func = obj_func_new(func_ctx->code_obj, func_ctx->params,
                                        new, func_ctx->frame, true);
    obj_set_attrib(new, key, new_func);
  } else {
    obj_set_attrib(new, key, attrib_val);
  }
}
struct obj *obj_instantiate(struct obj *class, struct vm *vm,
                            struct vec *args) {
  if (class->ref_immune) {
    return obj_invoke_attrib(class, "new", vm, args);
  }

  struct obj *new = obj_new(OBJ_ANON, NULL, class);
  hashmap_iterate(class->attribs, instantiate_attrib, new);

  obj_invoke_attrib(obj_inc_ref(new), "new", vm, args);
  obj_down_ref(new);

  return new;
}

struct obj *obj_invoke(struct obj *obj, struct vm *vm, struct vec *args) {
  bool no_args = false;

  if (args == NULL) {
    args = vec_new();
    no_args = true;
  }

  struct obj *ret;

  if (obj->type == OBJ_BUILTIN) {
    struct builtin_obj_ctx *builtin = obj->ctx;
    struct obj *self = builtin->self;
    if (self != NULL && self->type == OBJ_WEAKREF) {
      self = self->ctx;
    }
    struct stackframe frame;
    frame.invokee = obj;
    vec_push(vm->frames, &frame);
    ret = builtin->func(self, vm, args);
    vec_pop(vm->frames);
  } else if (obj->type == OBJ_FUNC) {
    struct func_obj_ctx *func = obj->ctx;
    struct stackframe *frame;
    if (func->frame == NULL) {
      frame = stackframe_new(func->code_obj->locals, obj);
    } else {
      frame = func->frame;
    }

    for (int i = 0; i < func->params->len; i++) {
      stackframe_set(frame, (uintptr_t)vec_get(func->params, i),
                     obj_inc_ref(vec_get(args, i)));
    }

    struct obj *self = func->self;
    if (self != NULL && self->type == OBJ_WEAKREF) {
      self = self->ctx;
    }

    vec_push(vm->frames, stackframe_inc_ref(frame));
    ret = vm_run(vm, func->code_obj, self);
    stackframe_dec_ref(vec_pop(vm->frames));

    obj_down_ref(ret);
  } else if (obj_hashmap_has(obj->attribs, "new")) {
    struct obj *new = obj_instantiate(obj, vm, args);
    ret = new;
  } else {
    printf("object %d was not invokable!\n", obj->type);
    struct obj *thing = obj_hashmap_get(obj->attribs, "new");
    exit(-1);
  }

  if (no_args) {
    vec_free(args);
  }

  return ret;
}

struct obj *obj_invoke_attrib(struct obj *obj, char *attrib, struct vm *vm,
                              struct vec *args) {
  struct obj *func = obj_hashmap_get(obj->attribs, attrib);
  if (func == &none_obj) {
    printf("No such attrib %s\n", attrib);
    exit(-1);
  }
  return obj_invoke(func, vm, args);
}

bool obj_is(struct obj *obj, struct obj *type_obj) {
  if (obj == type_obj || obj->obj_type == type_obj) return true;
  if (type_obj->type != OBJ_TYPE) return false;
  struct obj *parent = obj->obj_type->parent;
  while (parent != &none_obj && parent != NULL) {
    if (parent == type_obj) return true;
    parent = parent->parent;
  }
  return false;
}

void obj_set_attrib(struct obj *obj, char *name, struct obj *val) {
  if (obj->attribs == NULL) {
    obj->attribs = obj_hashmap_new();
  }

  if (obj->type == OBJ_BUILTIN) {
    struct builtin_obj_ctx *ctx = obj->ctx;
    if (ctx->name == NULL) {
      ctx->name = name;
    }
  }

  obj_dec_ref(obj_hashmap_get(obj->attribs, name));
  obj_hashmap_set(obj->attribs, name, obj_inc_ref(val));
}

void obj_store_index(struct obj *obj, struct obj *key, struct obj *val,
                     struct vm *vm) {
  struct obj *storeindex = obj_hashmap_get(obj->attribs, "__storeindex__");
  if (storeindex != &none_obj) {
    struct vec *args = vec_new();
    vec_push(args, key);
    vec_push(args, val);
    obj_invoke(storeindex, vm, args);
    vec_free(args);
  } else if (key->type == OBJ_STRING) {
    char *attrib = key->ctx;
    obj_hashmap_set(obj->attribs, attrib, val);
  } else {
    printf("Key must be of type string!\n");
    exit(-1);
  }
}

struct obj *obj_to_string(struct obj *obj, struct vm *vm) {
  if (obj->lazy_load_fn) {
    obj->lazy_load_fn(obj);
    obj->lazy_load_fn = NULL;
  }
  struct obj *toString = obj_hashmap_get(obj->attribs, "toString");
  if (toString == &none_obj) {
    return obj_string_new(obj->obj_type->ctx);
  }
  struct vec args;
  args.len = 0;
  struct obj *ret = obj_invoke(toString, vm, &args);
  return ret;
}

struct obj array_type_obj = {
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj bool_type_obj = {
    .ctx = "Bool",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj builtin_type_obj = {
    .ctx = "Builtin",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj error_type_obj = {
    .ctx = "Error",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &error_type_obj,
    .attribs = NULL,
};

struct obj func_type_obj = {
    .ctx = "Func",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj iter_type_obj = {
    .ctx = "Iter",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj none_type_obj = {
    .ctx = "none",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = NULL,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj number_type_obj = {
    .ctx = "Number",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj object_type_obj = {
    .ctx = "Object",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = NULL,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj string_type_obj = {
    .ctx = "String",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = NULL,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj type_type_obj = {
    .ctx = "Type",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &object_type_obj,
    .attribs = NULL,
};

struct obj weakref_type_obj = {
    .ctx = "Weakref",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj type_error_type_obj = {
    .ctx = "TypeError",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &error_type_obj,
    .obj_type = &type_type_obj,
};

struct obj none_obj = {
    .ctx = "none",
    .ref_immune = true,
    .type = OBJ_NONE,
    .parent = NULL,
    .obj_type = &none_type_obj,
    .attribs = NULL,
};

struct obj true_obj = {
    .ctx = "true",
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = &object_type_obj,
    .obj_type = &bool_type_obj,
};

struct obj false_obj = {
    .ctx = "false",
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = &object_type_obj,
    .obj_type = &bool_type_obj,
};
