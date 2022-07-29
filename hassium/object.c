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
    case OBJ_WEAKREF:
      if (obj->ctx != &none_obj) {
        struct obj *ref = obj->ctx;
        vec_remove(ref->weak_refs, &obj->ctx);
      }
      break;
    case OBJ_BUILTIN: {
      struct builtin_obj_ctx *builtin = obj->ctx;
      obj_dec_ref(builtin->self);
      free(builtin);
    } break;
    default:
      if (obj->ctx != NULL) free(obj->ctx);
      break;
  }

  if (obj->attribs != NULL) {
    size_t iter = 0;
    void *item;
    while (hashmap_iter(obj->attribs, &iter, &item)) {
      struct obj_hashmap_entry *entry = item;
      obj_dec_ref(entry->obj);
    }
    hashmap_free(obj->attribs);
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
  obj_hashmap_set(obj->attribs, name, obj_inc_ref(val));
}

struct obj *obj_to_string(struct obj *obj, struct vm *vm) {
  struct obj *toString = obj_hashmap_get(obj->attribs, "toString");
  if (toString == NULL) {
    printf("Object does not have toString()!");
    exit(-1);
  }
  struct vec *args = vec_new();
  struct obj *ret = obj_invoke(toString, vm, args);
  vec_free(args);
  return obj_inc_ref(ret);
}

static int compare(const void *a, const void *b, void *data) {
  const struct obj_hashmap_entry *ea = a;
  const struct obj_hashmap_entry *eb = b;
  return strcmp(ea->name, ea->name);
}

static bool iter(const void *item, void *udata) {
  const struct obj_hashmap_entry *entry = item;
  return true;
}

static uint64_t hash(const void *ptr, uint64_t seed0, uint64_t seed1) {
  const struct obj_hashmap_entry *item = ptr;
  return hashmap_sip(item->name, strlen(item->name), seed0, seed1);
}

struct hashmap *obj_hashmap_new() {
  return hashmap_new(sizeof(struct obj_hashmap_entry), 0, 0, 0, hash, compare,
                     NULL, NULL);
}

struct obj *obj_hashmap_get(struct hashmap *map, char *key) {
  struct obj_hashmap_entry *entry =
      hashmap_get(map, &(struct obj_hashmap_entry){.name = key});
  if (entry != NULL)
    return entry->obj;
  else
    return NULL;
}

void obj_hashmap_set(struct hashmap *map, char *key, struct obj *val) {
  hashmap_set(map, &(struct obj_hashmap_entry){.name = key, .obj = val});
}

void obj_hashmap_free(struct hashmap *map) {
  size_t iter = 0;
  void *item;
  while (hashmap_iter(map, &iter, &item)) {
    struct obj_hashmap_entry *entry = item;
    obj_dec_ref(entry->obj);
  }
  hashmap_free(map);
}
