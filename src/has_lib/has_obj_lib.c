#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_invoke (struct has_obj * obj, struct vm_state * vm, struct vector_state * args) {
    struct has_method * invokable;

    invokable = (struct has_method *)has_obj_get_attrib (obj, "_invoke")->state;

    return invokable->invoke (vm, invokable->self, args);
}

struct has_obj * has_obj_add (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj * _add;

    _add = has_obj_get_attrib (left, "_add");

    return has_obj_invoke (_add, vm, assemble_args (1, right));
}

char * has_obj_to_cstring (struct has_obj * obj, struct vm_state * vm) {
    struct has_string * str;
    struct has_obj    * to_string;

    to_string = has_obj_get_attrib (obj, "toString");
    str       = (struct has_string *)has_obj_invoke     (to_string, vm, NULL)->state;

    return str->val;
}

struct vector_state * assemble_args (int arg_count, ...) {
    struct vector_state * args;
    va_list ap;

    args = vector_init ();
    va_start (ap, arg_count);
    for (int i = 0; i < arg_count; i++) {
        vector_push (args, va_arg(ap, struct has_obj *));
    }

    va_end (ap);

    return args;
}
