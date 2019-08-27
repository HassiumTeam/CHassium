#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_add (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj * _add;

    _add = has_obj_get_attrib (left, "_add");

    return has_obj_invoke (vm, _add, assemble_args (1, right));
}

struct has_obj * has_obj_index (struct vm_state * vm, struct has_obj * obj, struct has_obj * index) {
    struct has_obj * _index;

    _index = has_obj_get_attrib (obj, "_index");

    return has_obj_invoke (vm, _index, assemble_args (1, index));
}

struct has_obj * has_obj_invoke (struct vm_state * vm, struct has_obj * obj, struct vector_state * args) {
    struct has_method * invokable;

    invokable = (struct has_method *)has_obj_get_attrib (obj, "_invoke")->state;

    return invokable->invoke (vm, invokable->self, args);
}

char * has_obj_to_cstring (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj    * str_obj;
    struct has_obj    * to_string;
    char              * ret;
    char              * str;

    to_string = has_obj_get_attrib (obj, "toString");
    str_obj   = has_obj_invoke (vm, to_string, NULL);
    str       = ((struct has_string *)(str_obj->state))->val;
    ret       = (char *)calloc (strlen (str) + 1, sizeof (char));

    memcpy (ret, str, strlen (str) + 1);
    has_obj_free (str_obj);

    return ret;
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
