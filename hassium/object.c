#include <object.h>

static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_new(obj_ctx_type_t type, void *ctx, struct obj *obj_type) {
  struct obj *obj = (struct obj *)malloc(sizeof(struct obj));
  obj->refs = 0;
  obj->ref_immune = false;
  obj->type = type;
  obj->ctx = ctx;
  obj->obj_type = obj_inc_ref(obj_type);
  obj->ops = NULL;
  obj->parent = &object_type_obj;
  obj->attribs = obj_hashmap_new();
  obj->weak_refs = NULL;

  return obj;
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
  struct obj *ret;
  struct vec *args = vec_new();
  vec_push(args, right);
  switch (type) {
    case BIN_OP_ADD:
      if (left->ops != NULL) {
        ret = left->ops->__add__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__add__", vm, args);
      }
      break;
    case BIN_OP_AND:
      ret = bool_to_obj(obj_is_true(left, vm) && obj_is_true(right, vm));
    case BIN_OP_DIV:
      if (left->ops != NULL) {
        ret = left->ops->__div__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__div__", vm, args);
      }
      break;
    case BIN_OP_EQ:
      if (left == right) {
        ret = &true_obj;
      } else if (left->ops != NULL) {
        ret = left->ops->__eq__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__eq__", vm, args);
      }
      break;
    case BIN_OP_GREATER:
      if (left->ops != NULL) {
        ret = left->ops->__greater__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__greater__", vm, args);
      }
      break;
    case BIN_OP_GREATER_OR_EQ:
      if (obj_bin_op(BIN_OP_EQ, left, right, vm) == &true_obj) {
        ret = &true_obj;
      } else {
        if (left->ops != NULL) {
          ret = left->ops->__add__(left, vm, args);
        } else {
          ret = obj_invoke_attrib(left, "__add__", vm, args);
        }
      }
      break;
    case BIN_OP_LESSER:
      if (left->ops != NULL) {
        ret = left->ops->__lesser__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__lesser__", vm, args);
      }
      break;
    case BIN_OP_LESSER_OR_EQ:
      if (obj_bin_op(BIN_OP_EQ, left, right, vm) == &true_obj) {
        ret = &true_obj;
      } else {
        if (left->ops != NULL) {
          ret = left->ops->__lesser__(left, vm, args);
        } else {
          ret = obj_invoke_attrib(left, "__lesser__", vm, args);
        }
      }
      break;
    case BIN_OP_MOD:
      if (left->ops != NULL) {
        ret = left->ops->__mod__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__mod__", vm, args);
      }
      break;
    case BIN_OP_MUL:
      if (left->ops != NULL) {
        ret = left->ops->__mul__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__mul__", vm, args);
      }
      break;
    case BIN_OP_OR:
      ret = bool_to_obj(obj_is_true(left, vm) || obj_is_true(right, vm));
      break;
    case BIN_OP_SUB:
      if (left->ops != NULL) {
        ret = left->ops->__sub__(left, vm, args);
      } else {
        ret = obj_invoke_attrib(left, "__sub__", vm, args);
      }
      break;
  }

  vec_free(args);
  return ret;
}

struct obj *obj_eq(struct obj *left, struct obj *right, struct vm *vm) {
  if (obj_hashmap_has(left->attribs, "__eq__")) {
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
    struct vec *key_args = vec_new();
    vec_push(key_args, key);
    struct obj *val = obj_invoke(index, vm, key_args);
    vec_free(key_args);
    return val;
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
    struct obj *new_func =
        obj_func_new(func_ctx->code_obj, func_ctx->params, new,
                     stackframe_inc_ref(func_ctx->frame));
    obj_set_attrib(new, key, new_func);
  } else {
    obj_set_attrib(new, key, attrib_val);
  }
}
struct obj *obj_instantiate(struct obj *class, struct vm *vm,
                            struct vec *args) {
  struct obj *new = obj_new(OBJ_ANON, NULL, class);
  hashmap_iterate(class->attribs, instantiate_attrib, new);
  if (obj_hashmap_has(new->attribs, "new")) {
    obj_invoke_attrib(obj_inc_ref(new), "new", vm, args);
    obj_down_ref(new);
  }
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
    ret = builtin->func(self, vm, args);
  } else if (obj->type == OBJ_FUNC) {
    struct func_obj_ctx *func = obj->ctx;
    struct stackframe *frame;
    if (func->frame == NULL) {
      frame = stackframe_new(func->code_obj->locals);
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
    struct stackframe *f = stackframe_dec_ref(vec_pop(vm->frames));
    obj_down_ref(ret);
  } else if (obj_hashmap_has(obj->attribs, "new")) {
    struct obj *new = obj_instantiate(obj, vm, args);
    ret = new;
  } else {
    printf("object %d was not invokable!\n", obj->type);
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

  struct obj *parent = obj->parent;
  while (parent != &none_obj && parent != NULL) {
    if (parent->obj_type == type_obj) return true;
    parent = parent->parent;
  }
  return false;
}

void obj_set_attrib(struct obj *obj, char *name, struct obj *val) {
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
  struct obj *toString = obj_hashmap_get(obj->attribs, "toString");
  if (toString == &none_obj) {
    return obj_string_new(obj->obj_type->ctx);
  }
  struct vec *args = vec_new();
  struct obj *ret = obj_invoke(toString, vm, args);
  vec_free(args);
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
    .ctx = "bool",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj builtin_type_obj = {
    .ctx = "builtin",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj func_type_obj = {
    .ctx = "func",
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
    .ctx = "number",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj object_type_obj = {
    .ctx = "object",
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
    .ctx = "type",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &object_type_obj,
    .attribs = NULL,
};

struct obj weakref_type_obj = {
    .ctx = "weakref",
    .ref_immune = true,
    .type = OBJ_TYPE,
    .parent = &object_type_obj,
    .obj_type = &type_type_obj,
    .attribs = NULL,
};

struct obj none_obj = {
    .ref_immune = true,
    .type = OBJ_NONE,
    .parent = NULL,
    .obj_type = &none_type_obj,
    .attribs = NULL,
};

struct obj true_obj = {
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = &object_type_obj,
    .obj_type = &bool_type_obj,
};

struct obj false_obj = {
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = &object_type_obj,
    .obj_type = &bool_type_obj,
};
