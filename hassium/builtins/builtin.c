#include <builtins.h>

struct obj *obj_builtin_new(builtin_func_t func, struct obj *self) {
  struct builtin_obj_ctx *ctx =
      (struct builtin_obj_ctx *)malloc(sizeof(struct builtin_obj_ctx));
  ctx->func = func;
  if (self != NULL) {
    ctx->self = obj_inc_ref(obj_weakref_new(self));
  } else {
    ctx->self = NULL;
  }
  struct obj *builtin = obj_new(OBJ_BUILTIN, ctx, &func_type_obj);
  return builtin;
}