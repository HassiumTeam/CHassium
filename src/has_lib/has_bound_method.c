#include <has_lib/has_bound_method.h>

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_bound_method_init (struct has_obj * method, struct has_obj * parent) {
    struct has_bound_method * state;
    struct has_obj          * obj;

    state         = (struct has_bound_method *)calloc (1, sizeof (struct has_bound_method));
    state->method = method;
    state->parent = parent;

    obj = has_obj_init (get_func_type (), state, has_bound_method_free);
    has_obj_set_attrib (obj, "_invoke", has_method_init (obj, _invoke, NULL));

    return obj;
}

void has_bound_method_free (void * state) {
    struct has_bound_method * bound_method = state;

    gc_remove_ref (bound_method->parent);
    free (state);
}

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_bound_method * this;

    this = (struct has_bound_method *)self->state;

    return has_obj_invoke (vm, this->method, args);
}
