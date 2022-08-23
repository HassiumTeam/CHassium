#include <builtins.h>

struct obj *obj_iter_new(struct obj *target) {
  struct iter_obj_ctx *ctx = malloc(sizeof(struct iter_obj_ctx));
  ctx->target = obj_inc_ref(target);
  ctx->pos = 0;
  return obj_new(OBJ_ITER, ctx, &iter_type_obj);
}