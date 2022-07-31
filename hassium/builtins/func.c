#include <builtins.h>

struct obj *obj_func_new(struct code_obj *code_obj, struct vec *params) {
  struct func_obj_ctx *ctx =
      (struct func_obj_ctx *)malloc(sizeof(struct func_obj_ctx));
  ctx->code_obj = code_obj;
  ctx->params = params;
  struct obj *func = obj_new(OBJ_FUNC, ctx, &func_type_obj);
  return func;
}