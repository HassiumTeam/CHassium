#include <has_lib/modules/default/number.h>

static struct has_obj * _add               (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _bitwise_and       (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _bitwise_or        (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _divide            (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _equal             (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _greater           (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _greater_or_equal  (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _lesser            (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _lesser_or_equal   (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _logical_and       (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _logical_or        (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _mod               (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _mul               (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_number          (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_string          (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_number_init (float f) {
    struct has_number * state;
    struct has_obj    * obj;

    state      = (struct has_number *)calloc (1, sizeof (struct has_number));
    state->val = f;

    obj = has_obj_init (get_number_type (), state, has_number_free);
    has_obj_set_attrib (obj, "_add",              has_method_init (obj, _add,         NULL));
    has_obj_set_attrib (obj, "_bitwise_and",      has_method_init (obj, _bitwise_and, NULL));
    has_obj_set_attrib (obj, "_bitwise_or",       has_method_init (obj, _bitwise_or,  NULL));
    has_obj_set_attrib (obj, "_divide",           has_method_init (obj, _divide,      NULL));
    has_obj_set_attrib (obj, "_equal",            has_method_init (obj, _equal,       NULL));
    has_obj_set_attrib (obj, "_greater",          has_method_init (obj, _greater,     NULL));
    has_obj_set_attrib (obj, "_greater_or_equal", has_method_init (obj, _greater,     NULL));
    has_obj_set_attrib (obj, "_lesser",           has_method_init (obj, _greater,     NULL));
    has_obj_set_attrib (obj, "_lesser_or_equal",  has_method_init (obj, _greater,     NULL));
    has_obj_set_attrib (obj, "_logical_and",      has_method_init (obj, _logical_and, NULL));
    has_obj_set_attrib (obj, "_logical_or",       has_method_init (obj, _logical_or,  NULL));
    has_obj_set_attrib (obj, "_mod",              has_method_init (obj, _mod,         NULL));
    has_obj_set_attrib (obj, "_mul",              has_method_init (obj, _mul,         NULL));
    has_obj_set_attrib (obj, "toNumber",          has_method_init (obj, to_number,    NULL));
    has_obj_set_attrib (obj, "toString",          has_method_init (obj, to_string,    NULL));

    return obj;
}

void has_number_free (void * state) {
    struct has_number * number = state;
    free (number);
}

static struct has_obj * number_type = NULL;
struct has_obj * get_number_type () {
    if (number_type == NULL) {
        number_type = has_type_init ("number");
    }

    return number_type;
}

static struct has_obj * _add (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_obj    * arg;
    struct has_number * arg_as_num;
    char              * arg_as_str;
    char              * this_as_str;
    char              * res_str;

    this = (struct has_number *)self->state;
    arg = vector_get (args, 0);

    if (has_obj_instanceof (vm, arg, get_number_type ()) == HAS_TRUE) {
        arg_as_num = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;
        return has_number_init (this->val + arg_as_num->val);
    } else {
        arg_as_str  = has_obj_to_cstring (vm, arg);
        this_as_str = has_obj_to_cstring (vm, self);
        this_as_str = (char *)realloc (this_as_str, (strlen (arg_as_str) + strlen (this_as_str) + 1) * sizeof (char));
        memcpy (this_as_str + strlen (this_as_str), arg_as_str, strlen (arg_as_str) + 1);
        free (arg_as_str);

        return has_string_init (this_as_str);
    }
}

static struct has_obj * _bitwise_and (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * num;
    struct has_number * right;

    num   = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return has_number_init ((int)num->val & (int)right->val);
}

static struct has_obj * _bitwise_or (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * num;
    struct has_number * right;

    num   = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return has_number_init ((int)num->val | (int)right->val);
}

static struct has_obj * _divide (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * right;

    this  = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return has_number_init (this->val / right->val);
}

static struct has_obj * _equal (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * right;

    this  = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    if (this->val == right->val) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _greater (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * arg;

    this = (struct has_number *)self->state;
    arg  = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    if (this->val > arg->val) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _greater_or_equal (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * arg;

    this = (struct has_number *)self->state;
    arg  = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    if (this->val >= arg->val) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _lesser (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * arg;

    this = (struct has_number *)self->state;
    arg  = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    if (this->val < arg->val) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _lesser_or_equal (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * arg;

    this = (struct has_number *)self->state;
    arg  = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    if (this->val <= arg->val) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _logical_and (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_obj    * arg;

    arg  = (struct has_obj *)vector_get (args, 0);

    if (self != HAS_FALSE && arg != HAS_FALSE) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _logical_or (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_obj    * arg;

    arg  = (struct has_obj *)vector_get (args, 0);

    if (self != HAS_FALSE || arg != HAS_FALSE) {
        return HAS_TRUE;
    }

    return HAS_FALSE;
}

static struct has_obj * _mod (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * right;

    this  = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return has_number_init ((int)this->val % (int)right->val);
}

static struct has_obj * _mul (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * this;
    struct has_number * right;

    this  = (struct has_number *)self->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return has_number_init (this->val * right->val);
}

static struct has_obj * to_number (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    return self;
}

static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * num;
    char              * buf;

    num = (struct has_number *)self->state;

    buf = (char *)calloc (1, 100);
    sprintf (buf, "%f", num->val);

    return has_string_init (buf);
}
