#include <has_lib/has_func.h>

struct has_obj * has_func_init (char * name) {
    struct has_func * state;
    struct has_obj  * obj;

    state = (struct has_func *)calloc (1, sizeof (struct has_func));
    state->name = name;

    obj = has_obj_init (state);

    return obj;
}

void has_func_free (struct has_obj * obj) {
    struct has_func * state = obj->state;

    free (state->name);

    has_obj_free (obj);
}
