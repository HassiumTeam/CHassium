#include <has_lib/has_func.h>

struct has_obj * has_func_init () {
    struct has_func * state;
    struct has_obj  * obj;

    state = (struct has_func *)calloc (1, sizeof (struct has_func));
    state->instructions = vector_init ();
    state->labels       = int_dict_init ();

    obj = has_obj_init (state);

    return obj;
}

void has_func_free (struct has_obj * obj) {
    struct has_func * state = obj->state;

    vector_free (state->instructions);

    has_obj_free (obj);
}

void emit (struct has_obj * obj, struct inst * inst) {
    struct has_func * state = (struct has_func *)obj->state;

    vector_push (state->instructions, inst);
}

void emit_label (struct has_obj * obj, int label) {
    struct has_func * state = (struct has_func *)obj->state;

    int_dict_set (
        state->labels,
        label,
        state->instructions->length
    );
}
