#include <has_lib/has_class.h>

struct has_obj * has_class_init (char * name, struct vector_state * extends) {
    struct has_class * state;
    struct has_obj   * obj;

    state          = (struct has_class *)calloc (1, sizeof (struct has_class));
    state->name    = name;
    state->extends = extends;

    obj = has_obj_init (NULL, state, has_class_free);

    return obj;
}

void has_class_free (void * state) {
    struct has_class * class = (struct has_class *)state;

    if (class->extends) {
        for (int i = 0; i < class->extends->length; i++) {
            free (vector_get (class->extends, i));
        }
        vector_free (class->extends);
    }

    free (class->name);

    free (class);
}
