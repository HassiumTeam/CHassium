#include <has_lib/has_func.h>

struct has_obj * has_func_init (char * name, struct vector_state * params, struct vector_state * enforced_ret) {
    struct has_func * state;
    struct has_obj  * obj;

    state               = (struct has_func *)calloc (1, sizeof (struct has_func));
    state->name         = name;
    state->params       = params;
    state->enforced_ret = enforced_ret;

    obj = has_obj_init (state, has_func_free);

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
