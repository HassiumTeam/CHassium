#include <builtins.h>

struct obj *obj_weakref_new(struct obj *ref)
{
    if (ref->weak_refs == NULL)
        ref->weak_refs = vec_new();
    struct weakref_obj_ctx *ctx = (struct weakref_obj_ctx *)malloc(sizeof(struct weakref_obj_ctx));
    ctx->ref = ref;
    struct obj *weakref = obj_new(OBJ_WEAKREF, ctx);
    vec_push(ref->weak_refs, &weakref);
    return weakref;
}

struct obj *obj_weakref_val(struct obj *weakref)
{
    struct weakref_obj_ctx *ctx = weakref->ctx;
    return ctx->ref;
}