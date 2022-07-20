#include <object.h>

struct obj *func_obj_new(struct obj *parent, struct obj *(*func)(struct obj *, struct vm *, struct vec *))
{
    struct func_obj_ctx *ctx = (struct func_obj_ctx *)calloc(1, sizeof(struct func_obj_ctx));
    ctx->func = func;
    return obj_new(parent, OBJ_FUNC, ctx);
}
