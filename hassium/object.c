#include <object.h>

static long next_obj_id = 0;
struct obj *obj_new(struct obj *parent, obj_ctx_type_t type, void *ctx)
{
    struct obj *obj = (struct obj *)calloc(1, sizeof(struct obj));
    obj->id = next_obj_id++;
    obj->refs = 0;
    obj->type = type;
    obj->ctx = ctx;
    obj->parent = parent;
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
        return ((struct func_obj_ctx *)obj->ctx)->func(obj->parent, vm, args);
    }
    else
    {
        printf("Invoked something that wasn't a func\n");
        exit(-1);
    }
}

struct obj *obj_invoke_attrib(struct obj *obj, struct vm *vm, char *attrib, struct vec *args)
{
    struct obj *func = obj_hashmap_get(obj->attribs, attrib);
    if (func->type == OBJ_FUNC)
    {
        struct func_obj_ctx *ctx = func->ctx;
        return ctx->func(obj, vm, args);
    }
    else
    {
        printf("Attrib was not a func!\n");
        exit(-1);
    }
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

static bool iter(const void *item, void *udata)
{
    const struct obj_hashmap_entry *entry = item;
    return true;
}

static uint64_t hash(const void *ptr, uint64_t seed0, uint64_t seed1)
{
    const struct obj_hashmap_entry *item = ptr;
    return hashmap_sip(item->name, strlen(item->name), seed0, seed1);
}

struct hashmap *obj_hashmap_new()
{
    return hashmap_new(sizeof(struct obj_hashmap_entry), 0, 0, 0, hash, compare, NULL, NULL);
}

struct obj *obj_hashmap_get(struct hashmap *map, char *key)
{
    struct obj_hashmap_entry *entry = hashmap_get(map, &(struct obj_hashmap_entry){.name = key});
    if (entry != NULL)
        return entry->obj;
    else
        return NULL;
}

void obj_hashmap_set(struct hashmap *map, char *key, struct obj *val)
{
    hashmap_set(map, &(struct obj_hashmap_entry){.name = key, .obj = val});
}
