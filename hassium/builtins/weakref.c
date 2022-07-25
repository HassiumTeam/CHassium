#include <builtins.h>

struct obj *obj_weakref_new(struct obj *ref)
{
    if (ref->weak_refs == NULL)
        ref->weak_refs = vec_new();
    struct obj *weakref = obj_new(OBJ_WEAKREF, NULL);
    weakref->ctx = ref;
    vec_push(ref->weak_refs, &weakref->ctx);
    printf("&weakref->ctx == %p\n", &weakref->ctx);
    return weakref;
}

struct obj *obj_weakref_val(struct obj *weakref)
{
    struct weakref_obj_ctx *ctx = weakref->ctx;
    return ctx->ref;
}