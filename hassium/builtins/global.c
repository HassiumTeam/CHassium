#include <builtins.h>

static struct obj *println(struct obj *obj, struct vm *vm, struct vec *args);

struct hashmap *get_defaults()
{
    struct hashmap *map = obj_hashmap_new();
    obj_hashmap_set(map, "println", func_obj_new(NULL, println));
    return map;
};

static struct obj *println(struct obj *obj, struct vm *vm, struct vec *args)
{
    for (int i = 0; i < args->len; i++)
    {
        struct obj *arg = vec_get(args, i);
        switch (arg->type)
        {
        case OBJ_STRING:
            printf("%s\n", ((struct string_obj_ctx *)arg->ctx)->value);
            break;
        default:
            printf("%s\n", ((struct string_obj_ctx *)obj_invoke_attrib(arg, vm, "toString", NULL)->ctx)->value);
            break;
        }
    }

    return obj_new(NULL, OBJ_ANON, NULL);
}