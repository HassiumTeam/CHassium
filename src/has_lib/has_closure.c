#include <has_lib/has_closure.h>

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_closure_init (struct has_obj * func, struct frame * frame) {
    struct has_closure * state;
    struct has_obj     * obj;

    state        = (struct has_closure *)calloc (1, sizeof (struct has_closure));
    state->func  = func;
    state->frame = frame;

    obj = has_obj_init (get_func_type (), state, has_closure_free);
    has_obj_set_attrib (obj, "_invoke", has_method_init (obj, _invoke, NULL));

    return obj;
}

void has_closure_free (void * state) {
    struct has_closure * closure = state;

    gc_remove_ref (closure->func);
    free (closure);
}

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_closure * this;
    struct has_obj     * ret;

    this = (struct has_closure *)self->state;

    push_frame (vm->stack_frame, this->frame);
    ret = has_obj_invoke (vm, this->func, args);
    vector_pop (vm->stack_frame->frames);

    return ret;
}
