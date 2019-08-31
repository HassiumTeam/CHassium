#include <has_lib/modules/default/number.h>

static struct has_obj * _add (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_number (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_number_init (float f) {
    struct has_number * state;
    struct has_obj    * obj;

    state      = (struct has_number *)calloc (1, sizeof (struct has_number));
    state->val = f;

    obj = has_obj_init (get_number_type (), state, has_number_free);
    has_obj_set_attrib (obj, "_add",     has_method_init (obj, _add,      NULL));
    has_obj_set_attrib (obj, "toNumber", has_method_init (obj, to_number, NULL));
    has_obj_set_attrib (obj, "toString", has_method_init (obj, to_string, NULL));

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
    struct has_number * num;
    struct has_number * right;

    num   = (struct has_number *)self                                    ->state;
    right = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    vector_free (args);

    return has_number_init (num->val + right->val);
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

    if (args != NULL) {
        vector_free (args);
    }

    return has_string_init (buf);
}
