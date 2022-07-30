#include <object.h>

static long next_obj_id = 3;
struct obj none_obj = {
    .id = 0,
    .ref_immune = true,
    .type = OBJ_NONE,
    .parent = NULL,
};

struct obj true_obj = {
    .id = 1,
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = NULL,
};

struct obj false_obj = {
    .id = 2,
    .ref_immune = true,
    .type = OBJ_BOOL,
    .parent = NULL,
};

struct obj *obj_new(obj_ctx_type_t type, void *ctx) {
  struct obj *obj = (struct obj *)malloc(sizeof(struct obj));
  obj->id = next_obj_id++;
  obj->refs = 0;
  obj->ref_immune = false;
  obj->type = type;
  obj->ctx = ctx;
  obj->parent = NULL;
  obj->attribs = obj_hashmap_new();
  obj->weak_refs = NULL;
  return obj;
}

void obj_free(struct obj *obj) {
  // printf("freeing %d\n", obj->type);
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
      for (int i = 0; i < items->len; i++) obj_dec_ref(vec_get(items, i));
      vec_free(items);
    } break;
    case OBJ_BUILTIN: {
      struct builtin_obj_ctx *builtin = obj->ctx;
      obj_dec_ref(builtin->self);
      free(builtin);
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
    default:
      if (obj->ctx != NULL) free(obj->ctx);
      break;
  }

  if (obj->attribs != NULL) {
    obj_hashmap_free(obj->attribs);
  }

  if (obj->parent != NULL) obj_dec_ref(obj->parent);
  free(obj);
}

struct obj *obj_bin_op(bin_op_type_t type, struct obj *left, struct obj *right,
                       struct vm *vm) {
  struct obj *ret;
  struct obj *func;
  struct vec *args = vec_new();
  vec_push(args, right);

  switch (type) {
    case BIN_OP_ADD:
      func = obj_hashmap_get(left->attribs, "__add__");
      break;
    case BIN_OP_AND:
      vec_free(args);
      return bool_to_obj(obj_is_true(left, vm) && obj_is_true(right, vm));
    case BIN_OP_DIV:
      func = obj_hashmap_get(left->attribs, "__div__");
      break;
    case BIN_OP_EQ:
      func = obj_hashmap_get(left->attribs, "__eq__");
      break;
    case BIN_OP_GREATER:
      func = obj_hashmap_get(left->attribs, "__greater__");
      break;
    case BIN_OP_MUL:
      func = obj_hashmap_get(left->attribs, "__mul__");
      break;
    case BIN_OP_OR:
      vec_free(args);
      return bool_to_obj(obj_is_true(left, vm) || obj_is_true(right, vm));
      break;
    case BIN_OP_SUB:
      func = obj_hashmap_get(left->attribs, "__sub__");
      break;
  }

  ret = obj_invoke(func, vm, args);
  vec_free(args);
  return ret;
}

struct obj *obj_inc_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs++;
  return obj;
}

struct obj *obj_dec_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs--;
  if (obj->refs <= 0) obj_free(obj);
  return obj;
}

struct obj *obj_down_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs--;
  return obj;
}

struct obj *obj_invoke(struct obj *obj, struct vm *vm, struct vec *args) {
  if (obj->type == OBJ_BUILTIN) {
    struct builtin_obj_ctx *builtin = obj->ctx;
    struct obj *self = builtin->self;
    if (self != NULL && self->type == OBJ_WEAKREF) self = self->ctx;
    return builtin->func(self, vm, args);
  } else if (obj->type == OBJ_FUNC) {
    struct func_obj_ctx *func = obj->ctx;
    struct hashmap *frame = obj_hashmap_new();
    for (int i = 0; i < func->params->len; i++)
      obj_hashmap_set(frame, vec_get(func->params, i),
                      obj_inc_ref(vec_get(args, i)));
    vec_push(vm->frames, frame);
    struct obj *ret = vm_run(vm, func->code_obj);
    obj_hashmap_free(vec_pop(vm->frames));
    return ret;
  } else {
    printf("object %d was not invokable!\n", obj->type);
    exit(-1);
  }
}

void obj_setattr(struct obj *obj, char *name, struct obj *val) {
  obj_dec_ref(obj_hashmap_get(obj->attribs, name));
  obj_hashmap_set(obj->attribs, name, obj_inc_ref(val));
}

struct obj *obj_subscript(struct obj *target, struct obj *key, struct vm *vm) {
  switch (target->type) {
    case OBJ_ARRAY:
      if (key->type != OBJ_NUM) {
        printf("Array must be indexed by a number!");
        exit(-1);
      }
      return vec_get(target->ctx, (int)obj_num_val(key));
    default:
      if (key->type != OBJ_STRING) {
        printf("Object must be accessed by a string!");
        exit(-1);
      }
      return obj_hashmap_get(target->attribs, obj_string_val(key));
  }
}

struct obj *obj_to_string(struct obj *obj, struct vm *vm) {
  struct obj *toString = obj_hashmap_get(obj->attribs, "toString");
  struct vec *args = vec_new();
  struct obj *ret = obj_invoke(toString, vm, args);
  vec_free(args);
  return obj_inc_ref(ret);
}

struct hashmap *obj_hashmap_new() {
  return hashmap_create();
}

struct obj *obj_hashmap_get(struct hashmap *map, char *key) {
  struct obj *obj;
  if (hashmap_get(map, key, strlen(key), (uintptr_t *)&obj)) {
    return obj;
  }
  return &none_obj;
}

void obj_hashmap_set(struct hashmap *map, char *key, struct obj *val) {
  hashmap_set(map, key, strlen(key), (uintptr_t)val);
}

static void obj_hashmap_entry_free(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  obj_dec_ref((struct obj *)value);
}

void obj_hashmap_free(struct hashmap *map) {
  hashmap_iterate(map, obj_hashmap_entry_free, NULL);
  hashmap_free(map);
}
