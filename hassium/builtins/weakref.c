#include <builtins.h>

struct obj *obj_weakref_new(struct obj *ref) {
  if (ref->weak_refs == NULL) {
    ref->weak_refs = vec_new();
  }
  struct obj *weakref = obj_new(OBJ_WEAKREF, NULL, &weakref_type_obj);
  weakref->ctx = ref;
  vec_push(ref->weak_refs, &weakref->ctx);
  return weakref;
}