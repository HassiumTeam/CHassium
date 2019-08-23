#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_invoke (struct has_obj * obj, struct vm_state * vm, struct vector_state * args) {
    struct has_method * invokable;

    invokable = (struct has_method *)has_obj_get_attrib (obj, "_invoke")->state;

    return invokable->invoke (vm, invokable->self, args);
}

char * has_obj_to_cstring (struct has_obj * obj, struct vm_state * vm) {
    struct has_string * str;
    struct has_obj    * to_string;

    to_string = has_obj_get_attrib (obj, "toString");
    str       = (struct has_string *)has_obj_invoke     (to_string, vm, NULL)->state;

    return str->val;
}
