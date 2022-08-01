#include <builtins.h>

struct obj *obj_func_new(struct code_obj *code_obj, struct vec *params,
                         struct obj *self) {
  struct func_obj_ctx *ctx =
      (struct func_obj_ctx *)malloc(sizeof(struct func_obj_ctx));
  ctx->code_obj = code_obj;
  ctx->params = params;
  if (self != NULL)
    ctx->self = obj_inc_ref(obj_weakref_new(self));
  else
    ctx->self = NULL;
  struct obj *func = obj_new(OBJ_FUNC, ctx, &func_type_obj);
  return func;
}