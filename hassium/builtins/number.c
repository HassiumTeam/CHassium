#include <builtins.h>

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_num_new(float value)
{
    struct num_obj_ctx *ctx = (struct num_obj_ctx *)malloc(sizeof(struct num_obj_ctx));
    ctx->value = value;
    struct obj *num = obj_new(OBJ_NUM, ctx);
    obj_setattr(num, "__add__", obj_builtin_new(__add__, num));
    obj_setattr(num, "toString", obj_builtin_new(toString, num));
    return num;
}

float obj_num_val(struct obj *obj)
{
    struct num_obj_ctx *ctx = obj->ctx;
    return ctx->value;
}

static struct obj *__add__(struct obj *num, struct vm *vm, struct vec *args)
{
    float left = obj_num_val(num);
    float right = obj_num_val(vec_get(args, 0));
    return obj_num_new(left + right);
}

static struct obj *toString(struct obj *num, struct vm *vm, struct vec *args)
{
    char str[64];
    sprintf(str, "%f", obj_num_val(num));
    return obj_string_new(str);
}