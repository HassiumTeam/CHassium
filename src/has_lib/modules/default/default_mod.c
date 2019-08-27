#include <has_lib/modules/default/default_mod.h>

static struct has_obj * instance = NULL;

static struct has_obj * has_null;

static struct has_obj * default_mod_init ();
static struct has_obj * println (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * get_default_mod () {
    if (instance == NULL) {
        instance = default_mod_init ();
    }

    return instance;
}

static struct has_obj * default_mod_init () {
    struct has_obj * obj;

    has_null = has_obj_init (NULL, NULL);

    obj = has_obj_init (NULL, NULL);

    has_obj_set_attrib (obj, "null",    has_null);
    has_obj_set_attrib (obj, "println", has_method_init (NULL, println));

    return obj;
}

static struct has_obj * println (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_obj * obj;
    char           * str;

    for (int i = 0; i < args->length; i++) {
        obj = vector_get (args, i);
        str = has_obj_to_cstring (vm, obj);
        printf ("%s", str);
        free (str);
    }

    printf ("\n");

    return has_null;
}
