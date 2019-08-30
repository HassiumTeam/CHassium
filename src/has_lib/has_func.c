#include <has_lib/has_func.h>

static struct has_obj * _invoke     (struct vm_state * vm, struct has_obj  * self, struct vector_state  * args);
static        void      import_args (struct vm_state * vm, struct has_func * state, struct vector_state * args);

struct has_obj * has_func_init (char * name, struct vector_state * params, struct vector_state * enforced_ret) {
    struct has_func * state;
    struct has_obj  * obj;

    state               = (struct has_func *)calloc (1, sizeof (struct has_func));
    state->name         = name;
    state->params       = params;
    state->enforced_ret = enforced_ret;

    obj = has_obj_init (state, has_func_free);
    has_obj_set_attrib (obj, "_invoke", has_method_init (obj, _invoke));

    return obj;
}

void has_func_free (void * state) {
    struct has_func * func = state;

    if (func->name) {
        free (func->name);
    }

    param_list_free (func->params);
    if (func->enforced_ret) {
        access_chain_free (func->enforced_ret);
    }
    free (func);
}

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_func * this;
    struct has_obj  * ret;

    this = (struct has_func *)self->state;

    push_frame (vm->stack_frame, NULL);

    import_args (vm, this, args);
    ret = vm_run (vm, self, NULL);
    
    pop_frame  (vm->stack_frame);

    return ret;
}

static void import_args (struct vm_state * vm, struct has_func * state, struct vector_state * args) {
    struct has_obj    * arg_val;
    struct func_param * param;

    for (int i = 0; i < state->params->length; i++) {
        arg_val = vector_get (args, i);
        param   = vector_get (state->params, i);

        switch (param->type) {
            case enforced_param:

                break;
            case object_param:

                break;
            case regular_param:
                set_var (vm->stack_frame, param->symbol, arg_val);
                break;
        }
    }
}
