#include <has_lib/modules/default/iter.h>

static struct has_obj * iter_full (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * iter_next (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_iter_init (struct vector_state * vals) {
    struct has_iter * state;
    struct has_obj  * obj;

    state = (struct has_iter *)calloc (1, sizeof (struct has_iter));
    state->vals = vals;
    state->pos  = 0;

    obj = has_obj_init (get_iter_type (), state, has_iter_free);
    has_obj_set_attrib (obj, "iter_full", has_method_init (obj, iter_full, NULL));
    has_obj_set_attrib (obj, "iter_next", has_method_init (obj, iter_next, NULL));

    return obj;
}

void has_iter_free (void * state) {
    struct has_iter * iter = state;

    vector_free (iter->vals);
    free        (iter);
}

static struct has_obj * iter_type = NULL;
struct has_obj * get_iter_type () {
    if (iter_type == NULL) {
        iter_type = has_type_init ("iter");
    }

    return iter_type;
}

static struct has_obj * iter_full (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_iter * this;

    if (args != NULL) {
        vector_free (args);
    }

    this = (struct has_iter *)(self->state);

    if (this->pos >= this->vals->length) {
        return has_number_init (1);
    } else {
        return has_number_init (0);
    }
}

static struct has_obj * iter_next (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_iter * this;

    if (args != NULL) {
        vector_free (args);
    }

    this = (struct has_iter *)(self->state);

    return vector_get (this->vals, this->pos++);
}
