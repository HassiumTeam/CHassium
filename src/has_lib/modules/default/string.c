#include <has_lib/modules/default/string.h>

static struct has_obj * _add      (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_string_init (char * val) {
    struct has_string * state;
    struct has_obj    * obj;

    state      = (struct has_string *)calloc (1, sizeof (struct has_string));
    state->val = val;

    obj = has_obj_init (get_string_type (), state, has_string_free);
    has_obj_set_attrib (obj, "_add",     has_method_init (obj, _add,      NULL));
    has_obj_set_attrib (obj, "toString", has_method_init (obj, to_string, NULL));

    return obj;
}

void has_string_free (void * state) {
    struct has_string * string = state;
    free (string->val);
    free (string);
}

static struct has_obj * string_type = NULL;
struct has_obj * get_string_type () {
    if (string_type == NULL) {
        string_type = has_type_init ("string");
    }

    return string_type;
}

static struct has_obj * _add (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    char * res;
    char * right;

    res   = has_obj_to_cstring (vm, self);
    right = has_obj_to_cstring (vm, vector_get (args, 0));
    res   = realloc (res, strlen (res) + strlen (right) + 1);
    strcat (res, right);

    free (right);

    return has_string_init (res);
}

static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    return self;
}
