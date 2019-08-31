#include <has_lib/modules/default/default_mod.h>

static struct has_obj * instance = NULL;

static struct has_obj * has_null;
static struct has_obj * has_true;
static struct has_obj * has_false;

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

    has_null  = has_type_init   ("NULL");
    has_true  = has_number_init (1);
    has_false = has_number_init (0);

    obj = has_obj_init (get_obj_type (), NULL, NULL);

    has_obj_set_attrib (obj, "false",   has_false);
    has_obj_set_attrib (obj, "func",    get_func_type ());
    has_obj_set_attrib (obj, "iter",    get_iter_type ());
    has_obj_set_attrib (obj, "list",    get_list_type ());
    has_obj_set_attrib (obj, "null",    has_null);
    has_obj_set_attrib (obj, "number",  get_number_type ());
    has_obj_set_attrib (obj, "object",  get_obj_type ());
    has_obj_set_attrib (obj, "string",  get_string_type ());
    has_obj_set_attrib (obj, "true",    has_true);
    has_obj_set_attrib (obj, "println", has_method_init (NULL, println, NULL));

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
