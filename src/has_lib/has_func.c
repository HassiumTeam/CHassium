#include <has_lib/has_func.h>

static struct has_obj * _invoke       (struct vm_state * vm, struct has_obj  * self,  struct vector_state  * args);
static struct has_obj * _invoke_bound (struct vm_state * vm, struct has_obj  * self,  struct vector_state  * args);
static        void      import_args   (struct vm_state * vm, struct has_func * state, struct vector_state  * args, int start_index);

struct has_obj * has_func_init (char * name, struct vector_state * params, struct vector_state * enforced_ret) {
    struct has_func * state;
    struct has_obj  * obj;

    state               = (struct has_func *)calloc (1, sizeof (struct has_func));
    state->name         = name;
    state->params       = params;
    state->enforced_ret = enforced_ret;

    obj = has_obj_init (get_func_type (), state, has_func_free);
    has_obj_set_attrib (obj, "_invoke",       has_method_init (obj, _invoke,       NULL));
    has_obj_set_attrib (obj, "_invoke_bound", has_method_init (obj, _invoke_bound, NULL));
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

    import_args (vm, this, args, 0);
    ret = vm_run (vm, self, NULL);

    pop_frame  (vm->stack_frame);

    return ret;
}

static struct has_obj * _invoke_bound (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_func * this;
    struct has_obj  * ret;

    this = (struct has_func *)self->state;

    push_frame (vm->stack_frame, NULL);

    import_args (vm, this, args, 1);
    ret = vm_run (vm, self, vector_get (args, 0));

    pop_frame  (vm->stack_frame);

    return ret;
}

static void import_args (struct vm_state * vm, struct has_func * state, struct vector_state * args, int start_index) {
    struct has_obj    * arg_val;
    struct func_param * param;
    struct has_obj    * enforced_type;

    for (int i = 0; i < state->params->length; i++) {
        arg_val = vector_get (args, i + start_index);
        param   = vector_get (state->params, i);
        switch (param->type) {
            case enforced_param:
                enforced_type = vm_resolve_access_chain (vm, param->enforced_type);
                if (has_obj_instanceof (vm, arg_val, enforced_type) == HAS_TRUE) {
                    set_var (vm->stack_frame, param->symbol, arg_val);
                } else {
                    printf ("TODO throw error here!\n");
                    exit (EXIT_FAILURE);
                }
                break;
            case object_param:
                for (int i = 0; i < param->ids->length; i++) {
                    set_var (
                        vm->stack_frame,
                        int_vector_get (param->symbols, i),
                        has_obj_get_attrib (arg_val, vector_get (param->ids, i))
                    );
                }
                break;
            case regular_param:
                set_var (vm->stack_frame, param->symbol, arg_val);
                break;
        }
    }
}
