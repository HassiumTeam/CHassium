#include <has_lib/has_bound_func.h>

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_bound_func_init (struct has_obj * func, struct has_obj * self) {
    struct has_bound_func * state;
    struct has_obj        * obj;

    state = (struct has_bound_func *)calloc (1, sizeof (struct has_bound_func));
    state->func = func;
    state->self = self;

    obj = has_obj_init (get_func_type (), state, has_bound_func_free);
    has_obj_set_attrib (obj, "_invoke", has_method_init (obj, _invoke, NULL));

    return obj;
}

void has_bound_func_free (void * state) {
    struct has_bound_func * bound_func = state;

    free (bound_func);
}

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_bound_func * this;
    struct has_obj        * _invoke_bound;
    struct vector_state   * new_args;
    struct has_obj        * ret;

    this          = (struct has_bound_func *)self->state;
    _invoke_bound = has_obj_get_attrib (this->func, "_invoke_bound");

    new_args = vector_init ();
    vector_push (new_args, this->self);
    for (int i = 0; i < args->length; i++) {
        vector_push (new_args, vector_get (args, i));
    }

    ret = has_obj_invoke (vm, _invoke_bound, new_args);
    vector_free (new_args);

    return ret;
}
