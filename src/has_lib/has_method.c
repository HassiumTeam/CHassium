#include <has_lib/has_method.h>

struct has_obj * has_method_init (struct has_obj * self, struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, struct vector_state * args), struct has_obj * type) {
    struct has_method * state;
    struct has_obj    * obj;

    state = (struct has_method *)calloc (1, sizeof (struct has_method));
    state->invoke = invoke;
    state->self   = self;

    if (type == NULL) {
        obj = has_obj_init (get_func_type (), state, has_method_free);
    } else {
        obj = has_obj_init (type, state, has_method_free);
    }

    has_obj_set_attrib (obj, "_invoke", obj);

    return obj;
}

void has_method_free (void * state) {
    free (state);
}

static struct has_obj * func_type = NULL;
struct has_obj * get_func_type () {
    if (func_type == NULL) {
        func_type = has_type_init ("func");
    }

    return func_type;
}
