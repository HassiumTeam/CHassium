#include <runtime/has_obj.h>

struct has_obj * has_obj_init (struct has_obj * type, void * state, void (* free_state) (void *)) {
    struct has_obj * obj;

    obj               = (struct has_obj *)calloc (1, sizeof (struct has_obj));
    obj->type         = type;
    obj->instructions = vector_init ();
    obj->ref_count    = 0;

    obj->state        = state;
    obj->free_state   = free_state;

    return obj;
}

void free_has_obj (struct has_obj * obj) {
    if (obj->state) {
        obj->free_state (obj->state);
    }

    for (int i = 0; i < obj->instructions->length; i++) {
        free_inst (vector_get (obj->instructions, i));
    }
    free_vector (obj->instructions);

    free (obj);
}
