#include <object.h>

static long next_obj_id = 0;
struct obj *obj_new(obj_ctx_type_t type, void *ctx)
{
    struct obj *obj = (struct obj *)calloc(1, sizeof(struct obj));
    obj->id = next_obj_id++;
    obj->refs = 0;
    obj->type = type;
    obj->ctx = ctx;
    obj->attribs = obj_hashmap_new();
    return obj;
}

void obj_free(struct obj *obj)
{
    free(obj);
}

struct obj *obj_inc_ref(struct obj *obj)
{
    obj->refs++;
    return obj;
}

struct obj *obj_dec_ref(struct obj *obj)
{
    obj->refs--;
    if (obj->refs <= 0)
        obj_free(obj);
}

void obj_setattrib(struct obj *obj, char *key, struct obj *val)
{
    obj_hashmap_set(obj->attribs, key, val);
}

struct obj *obj_invoke(struct obj *obj, struct vm *vm, struct vec *args)
{
    if (obj->type == OBJ_FUNC)
    {
        struct func_obj_ctx *ctx = obj->ctx;
        return ctx->func(obj, vm, args);
    }
    else
    {
        printf("not implemented yet?\n");
    }
}

struct obj *func_obj_new(struct obj *(*func)(struct obj *, struct vm *, struct vec *))
{
    struct func_obj_ctx *ctx = (struct func_obj_ctx *)calloc(1, sizeof(struct func_obj_ctx));
    ctx->func = func;
    return obj_new(OBJ_FUNC, ctx);
}

struct obj *num_obj_new(float value)
{
    struct num_obj_ctx *ctx = (struct num_obj_ctx *)calloc(1, sizeof(struct num_obj_ctx));
    ctx->value = value;
    return obj_new(OBJ_NUM, ctx);
}

struct obj *str_obj_new(char *value)
{
    struct string_obj_ctx *ctx = (struct string_obj_ctx *)calloc(1, sizeof(struct string_obj_ctx));
    ctx->value = value;
    return obj_new(OBJ_STRING, ctx);
}

struct obj_hashmap_entry
{
    char *name;
    struct obj *obj;
};

static int compare(const void *a, const void *b, void *data)
{
    const struct obj_hashmap_entry *ea = a;
    const struct obj_hashmap_entry *eb = b;
    return strcmp(ea->name, ea->name);
}

static uint64_t hash(const void *ptr, uint64_t seed0, uint64_t seed1)
{
    const struct obj_hashmap_entry *item = ptr;
    return hashmap_sip(item->name, strlen(item->name), seed0, seed1);
}

struct hashmap *obj_hashmap_new()
{
    return hashmap_new(sizeof(struct obj_hashmap_entry), 0, 0, 0, hash, NULL, NULL, NULL);
}

struct obj *obj_hashmap_get(struct hashmap *map, char *key)
{
    struct obj_hashmap_entry *entry = hashmap_get(map, key);
    return entry->obj;
}

void obj_hashmap_set(struct hashmap *map, char *key, struct obj *val)
{
    hashmap_set(map, &(struct obj_hashmap_entry){.name = key, .obj = val});
}
