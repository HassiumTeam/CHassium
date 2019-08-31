#include <has_lib/has_class.h>

struct has_obj * has_class_init (char * name, struct vector_state * extends) {
    struct has_class * state;
    struct has_obj   * obj;

    state          = (struct has_class *)calloc (1, sizeof (struct has_class));
    state->name    = name;
    state->extends = extends;
    state->type    = has_type_init (name);

    obj = has_obj_init (get_class_type (), state, has_class_free);

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

static struct has_obj * class_type = NULL;
struct has_obj * get_class_type () {
    if (class_type == NULL) {
        class_type = has_type_init ("class");
    }

    return class_type;
}
