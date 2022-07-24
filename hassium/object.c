#include <object.h>

static long next_obj_id = 1;
struct obj none_obj = {
    .id = 0,
    .refs = 1,
    .type = OBJ_NONE,
    .parent = NULL,
};

struct obj *obj_new(obj_ctx_type_t type, void *ctx)
{
    struct obj *obj = (struct obj *)calloc(1, sizeof(struct obj));
    obj->id = next_obj_id++;
    obj->refs = 0;
    obj->type = type;
    obj->ctx = ctx;
    obj->parent = NULL;
    obj->attribs = obj_hashmap_new();
    obj->weak_refs = NULL;
    return obj;
}

void obj_free(struct obj *obj)
{
    printf("Called obj_free on a %d at %p\n", obj->type, obj);
    if (obj->weak_refs != NULL)
    {
        struct obj **ref;
        for (int i = 0; i < obj->weak_refs->len; i++)
        {
            ref = vec_get(obj->weak_refs, i);
            *ref = &none_obj;
        }
        vec_free(obj->weak_refs);
    }

    struct weakref_obj_ctx *weakref;
    switch (obj->type)
    {
    case OBJ_WEAKREF:
        weakref = obj->ctx;
        if (weakref->ref != &none_obj)
        {
            printf("trying to remove weakrefs of object %d at %p\n", weakref->ref->type, weakref->ref);
            vec_remove(weakref->ref->weak_refs, &obj);
        }
        break;
    }

    if (obj->attribs != NULL)
    {
        size_t iter = 0;
        void *item;
        while (hashmap_iter(obj->attribs, &iter, &item))
        {
            struct obj_hashmap_entry *entry = item;
            obj_dec_ref(entry->obj);
        }
        hashmap_free(obj->attribs);
    }

    if (obj->parent != NULL)
        obj_dec_ref(obj->parent);
    if (obj->ctx != NULL)
    {
        free(obj->ctx);
    }
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

void obj_setattr(struct obj *obj, char *name, struct obj *val)
{
    obj_hashmap_set(obj->attribs, name, obj_inc_ref(val));
}

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
