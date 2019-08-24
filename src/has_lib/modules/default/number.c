#include <has_lib/modules/default/number.h>

static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_number_init (float f) {
    struct has_number * state;
    struct has_obj    * obj;

    state      = (struct has_number *)calloc (1, sizeof (struct has_number));
    state->val = f;

    obj = has_obj_init (state, has_number_free);
    has_obj_set_attrib (obj, "toString", has_method_init (obj, to_string));

    return obj;
}

void has_number_free (void * state) {
    struct has_number * number = state;

    free (number);
}

static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_number * num;
    char              * buf;

    num = (struct has_number *)self->state;

    buf = (char *)calloc (1, 100);
    sprintf (buf, "%f", num->val);

    return has_string_init (buf);
}
