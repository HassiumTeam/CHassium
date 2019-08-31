#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_add (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj * _add;

    _add = has_obj_get_attrib (left, "_add");

    return has_obj_invoke (vm, _add, assemble_args (1, right));
}

struct has_obj * has_obj_equal (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj * _equal;

    if (has_obj_has_attrib (left, "_equal")) {
        _equal = has_obj_get_attrib (left, "_equal");
        return has_obj_invoke (vm, _equal, assemble_args (1, right));
    }

    if (left == right) {
        return HAS_TRUE;
    } else {
        return HAS_FALSE;
    }
}

struct has_obj * has_obj_index (struct vm_state * vm, struct has_obj * obj, struct has_obj * index) {
    struct has_obj * _index;

    _index = has_obj_get_attrib (obj, "_index");

    return has_obj_invoke (vm, _index, assemble_args (1, index));
}

struct has_obj * has_obj_instanceof (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj * typeof_left;

    typeof_left = typeof_has_obj (vm, left);

    return has_obj_equal (vm, typeof_left, right);
}

struct has_obj * has_obj_iter (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj * _iter;

    _iter = has_obj_get_attrib (obj, "_iter");

    return has_obj_invoke (vm, _iter, NULL);
}

struct has_obj * has_obj_iter_full (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj * _iter_full;

    _iter_full = has_obj_get_attrib (obj, "iter_full");

    return has_obj_invoke (vm, _iter_full, NULL);
}

struct has_obj * has_obj_iter_next (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj * _iter_next;

    _iter_next = has_obj_get_attrib (obj, "iter_next");

    return has_obj_invoke (vm, _iter_next, NULL);
}

struct has_obj * has_obj_invoke (struct vm_state * vm, struct has_obj * obj, struct vector_state * args) {
    struct has_method * invokable;

    invokable = (struct has_method *)has_obj_get_attrib (obj, "_invoke")->state;

    return invokable->invoke (vm, invokable->self, args);
}

struct has_obj * has_obj_store_index (struct vm_state * vm, struct has_obj * obj, struct has_obj * index, struct has_obj * val) {
    struct has_obj * _store_index;

    _store_index = has_obj_get_attrib (obj, "_store_index");

    return has_obj_invoke (vm, _store_index, assemble_args (2, index, val));
}

struct has_obj * typeof_has_obj (struct vm_state * vm, struct has_obj * obj) {
    if (obj->type == NULL) {
        obj->type = get_type_type ();
    }

    return obj->type;
}

float has_obj_to_cfloat (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj * num_obj;
    struct has_obj * to_number;

    to_number = has_obj_get_attrib (obj, "toNumber");
    num_obj  = has_obj_invoke      (vm, to_number, NULL);

    if (num_obj != obj) {
        has_obj_free (num_obj);
    }

    return ((struct has_number *)num_obj->state)->val;
}

int has_obj_to_cint (struct vm_state * vm, struct has_obj * obj) {
    return (int)has_obj_to_cfloat (vm, obj);
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

    if (str_obj != obj) {
        has_obj_free (str_obj);
    }

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
