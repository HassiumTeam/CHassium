#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_invoke (struct has_obj * obj, struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_method * invokable;
    
    invokable = (struct has_method *)obj->state;

    return invokable->invoke (vm, self, args);
}
