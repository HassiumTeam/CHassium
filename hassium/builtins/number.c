#include <object.h>

static struct obj *num_obj_tostring(struct obj *obj, struct vm *vm, struct vec *args)
{
    struct num_obj_ctx *ctx = obj->ctx;
    char *str = (char *)calloc(16, sizeof(char));
    sprintf(str, "%f", ctx->value);
    return str_obj_new(str);
}

static struct obj *num_obj_add(struct obj *obj, struct vm *vm, struct vec *args)
{
    struct num_obj_ctx *left = obj->ctx;
    struct num_obj_ctx *right = ((struct num_obj_ctx *)((struct obj *)vec_get(args, 0))->ctx);
    struct obj *ret = num_obj_new(left->value + right->value);
    return ret;
}

struct obj *num_obj_new(float value)
{
    struct num_obj_ctx *ctx = (struct num_obj_ctx *)calloc(1, sizeof(struct num_obj_ctx));
    ctx->value = value;
    struct obj *obj = obj_new(NULL, OBJ_NUM, ctx);
    obj_setattrib(obj, "__add__", func_obj_new(obj, num_obj_add));
    obj_setattrib(obj, "toString", func_obj_new(obj, num_obj_tostring));
    return obj;
}