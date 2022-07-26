#include <builtins.h>

static struct obj *println(struct obj *, struct vm *, struct vec *);

struct hashmap *get_defaults()
{
    struct hashmap *defaults = obj_hashmap_new();
    obj_hashmap_set(defaults, "println", obj_inc_ref(obj_builtin_new(println, NULL)));
    return defaults;
}

static struct obj *println(struct obj *_, struct vm *vm, struct vec *args)
{
    for (int i = 0; i < args->len; i++)
    {
        struct obj *arg = vec_get(args, i);
        if (arg->type == OBJ_STRING)
        {
            printf("%s", (char *)arg->ctx);
        }
    }
    printf("\n");

    return &none_obj;
}