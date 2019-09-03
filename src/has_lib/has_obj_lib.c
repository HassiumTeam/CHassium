#include <has_lib/has_obj_lib.h>

struct has_obj * has_obj_add (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _add;
    struct vector_state * args;
    struct has_obj      * ret;

    _add = has_obj_get_attrib (left, "_add");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _add, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_bitwise_and (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _bitwise_and;
    struct vector_state * args;
    struct has_obj      * ret;

    _bitwise_and = has_obj_get_attrib (left, "_bitwise_and");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _bitwise_and, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_bitwise_or (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _bitwise_or;
    struct vector_state * args;
    struct has_obj      * ret;

    _bitwise_or = has_obj_get_attrib (left, "_bitwise_or");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _bitwise_or, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_divide (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _divide;
    struct vector_state * args;
    struct has_obj      * ret;

    _divide = has_obj_get_attrib (left, "_divide");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _divide, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_equal (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _equal;
    struct vector_state * args;
    struct has_obj      * ret;

    if (has_obj_has_attrib (left, "_equal")) {
        _equal = has_obj_get_attrib (left, "_equal");

        args   = assemble_args (1, right);
        ret    = has_obj_invoke (vm, _equal, args);
        vector_free (args);

        return ret;
    }

    if (left == right) {
        return HAS_TRUE;
    } else {
        return HAS_FALSE;
    }
}

struct has_obj * has_obj_greater (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _greater;
    struct vector_state * args;
    struct has_obj      * ret;

    _greater = has_obj_get_attrib (left, "_greater");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _greater, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_greater_or_equal (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _greater_or_equal;
    struct vector_state * args;
    struct has_obj      * ret;

    _greater_or_equal = has_obj_get_attrib (left, "_greater_or_equal");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _greater_or_equal, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_index (struct vm_state * vm, struct has_obj * obj, struct has_obj * index) {
    struct has_obj      * _index;
    struct vector_state * args;
    struct has_obj      * ret;

    _index = has_obj_get_attrib (obj, "_index");

    args = assemble_args  (1, index);
    ret  = has_obj_invoke (vm, _index, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_invoke (struct vm_state * vm, struct has_obj * obj, struct vector_state * args) {
    struct has_method * invokable;
    struct has_obj    * ret;
    int                 null_args = 0;

    invokable = (struct has_method *)has_obj_get_attrib (obj, "_invoke")->state;
    if (args == NULL) {
        args = vector_init ();
        null_args = 1;
    }

    ret = invokable->invoke (vm, invokable->self, args);
    if (null_args) {
        vector_free (args);
    }

    return ret;
}

struct has_obj * has_obj_instanceof (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * typeof_left;

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

struct has_obj * has_obj_lesser (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _lesser;
    struct vector_state * args;
    struct has_obj      * ret;

    _lesser = has_obj_get_attrib (left, "_lesser");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _lesser, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_lesser_or_equal (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _lesser_or_equal;
    struct vector_state * args;
    struct has_obj      * ret;

    _lesser_or_equal = has_obj_get_attrib (left, "_lesser_or_equal");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _lesser_or_equal, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_logical_and (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _logical_and;
    struct vector_state * args;
    struct has_obj      * ret;

    _logical_and = has_obj_get_attrib (left, "_logical_and");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _logical_and, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_logical_or (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _logical_or;
    struct vector_state * args;
    struct has_obj      * ret;

    _logical_or = has_obj_get_attrib (left, "_logical_or");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _logical_or, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_mod (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _mod;
    struct vector_state * args;
    struct has_obj      * ret;

    _mod = has_obj_get_attrib (left, "_mod");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _mod, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_mul (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _mul;
    struct vector_state * args;
    struct has_obj      * ret;

    _mul = has_obj_get_attrib (left, "_mul");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _mul, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_store_index (struct vm_state * vm, struct has_obj * obj, struct has_obj * index, struct has_obj * val) {
    struct has_obj      * _store_index;
    struct vector_state * args;
    struct has_obj      * ret;

    _store_index = has_obj_get_attrib (obj, "_store_index");

    args = assemble_args (2, index, val);
    ret = has_obj_invoke (vm, _store_index, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_sub (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _sub;
    struct vector_state * args;
    struct has_obj      * ret;

    _sub = has_obj_get_attrib (left, "_sub");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _sub, args);
    vector_free (args);

    return ret;
}

struct has_obj * has_obj_xor (struct vm_state * vm, struct has_obj * left, struct has_obj * right) {
    struct has_obj      * _xor;
    struct vector_state * args;
    struct has_obj      * ret;

    _xor = has_obj_get_attrib (left, "_xor");

    args = assemble_args  (1, right);
    ret  = has_obj_invoke (vm, _xor, args);
    vector_free (args);

    return ret;
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

    to_number = has_obj_get_attrib  (obj, "toNumber");
    num_obj   = has_obj_invoke      (vm, to_number, NULL);

    if (num_obj != obj) {
        has_obj_free (num_obj);
    }

    return ((struct has_number *)num_obj->state)->val;
}

int has_obj_to_cint (struct vm_state * vm, struct has_obj * obj) {
    return (int)has_obj_to_cfloat (vm, obj);
}

char * has_obj_to_cstring (struct vm_state * vm, struct has_obj * obj) {
    struct has_obj  * str_obj;
    struct has_obj  * to_string;
    char            * ret;
    char            * str;

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
