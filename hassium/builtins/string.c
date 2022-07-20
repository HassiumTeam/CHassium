#include <object.h>

struct obj *str_obj_new(char *value)
{
    struct string_obj_ctx *ctx = (struct string_obj_ctx *)calloc(1, sizeof(struct string_obj_ctx));
    ctx->value = value;
    return obj_new(NULL, OBJ_STRING, ctx);
}