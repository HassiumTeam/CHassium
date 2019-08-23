#include <has_lib/modules/default/default_mod.h>

struct has_obj * has_null;

struct has_obj * default_mod_init () {
    struct has_obj * obj;

    has_null = has_obj_init (NULL, NULL);

    obj = has_obj_init (NULL, NULL);
    has_obj_set_attrib (obj, "println", has_method_init (println));

    return obj;
}

struct has_obj * println (struct vm_state * vm, struct has_obj * self, int arg_count, ...) {
    struct has_obj * obj;
    va_list          ap;

    va_start (ap, arg_count);

    for (int i = 0; i < arg_count; i++) {
        obj = va_arg (ap, struct has_obj *);
        printf ("Test\n");
    }

    va_end   (ap);
}
