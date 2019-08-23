#include <has_lib/modules/default/default_mod.h>

struct has_obj * has_null;

struct has_obj * default_mod_init () {
    struct has_obj * obj;

    has_null = has_obj_init (NULL, NULL);

    obj = has_obj_init (NULL, NULL);
    has_obj_set_attrib (obj, "println", has_method_init (println));

    return obj;
}

struct has_obj * println (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_obj * obj;
    for (int i = 0; i < args->length; i++) {
        obj = vector_get (args, i);
    }

    vector_free (args);
}
