#include <builtins.h>

static void obj_builtin_lazy_load(struct obj *);

static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_builtin_new(builtin_func_t func, struct obj *self) {
  struct builtin_obj_ctx *ctx = malloc(sizeof(struct builtin_obj_ctx));
  ctx->func = func;
  if (self != NULL) {
    ctx->self = obj_inc_ref(obj_weakref_new(self));
  } else {
    ctx->self = NULL;
  }
  ctx->name = NULL;

  struct obj *builtin = obj_new(OBJ_BUILTIN, ctx, &func_type_obj);
  builtin->lazy_load_fn = obj_builtin_lazy_load;

  return builtin;
}

static void obj_builtin_lazy_load(struct obj *builtin) {
  obj_set_attrib(builtin, "toString", obj_builtin_new(toString, builtin));
}

static struct obj *toString(struct obj *obj, struct vm *vm, struct vec *args) {
  struct builtin_obj_ctx *builtin = obj->ctx;

  return obj_string_new(builtin->name);
}