#include <has_lib/has_type.h>

static struct has_obj * _equal    (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_type_init (char * name) {
    struct has_type * state;
    struct has_obj  * obj;

    state = (struct has_type *)calloc (1, sizeof (struct has_type));
    state->name = name;


    obj = has_obj_init (NULL, state, has_type_free);

    if (strcmp (name, "func") == 0) {
        has_obj_set_attrib (obj, "_equal",   has_method_init (obj, _equal,    obj));
        has_obj_set_attrib (obj, "toString", has_method_init (obj, to_string, obj));
    } else {
        has_obj_set_attrib (obj, "_equal",   has_method_init (obj, _equal,    NULL));
        has_obj_set_attrib (obj, "toString", has_method_init (obj, to_string, NULL));
    }

    return obj;
}

void has_type_free (void * state) {
    struct has_type * type = state;

    free (type);
}

static struct has_obj * type_type;
struct has_obj * get_type_type () {
    if (type_type == NULL) {
        type_type = has_type_init ("type");
    }

    return type_type;
}

static struct has_obj * _equal (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_type  * this;
    struct has_obj   * right;
    struct has_class * class;

    this  = (struct has_type *)self->state;
    right = vector_get (args, 0);

    if (self == right) {
        return HAS_TRUE;
    }

    if (typeof_has_obj (vm, right) == get_class_type ()) {
        class = (struct has_class *)(right->state);
        if (self == class->type) {
            return HAS_TRUE;
        } else {
            return HAS_FALSE;
        }
    }

    return HAS_FALSE;
}

static struct has_obj * to_string (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_type * this;
    char            * name;

    this = (struct has_type *)self->state;

    name = (char *)calloc (strlen (this->name) + 1, sizeof (char));
    memcpy (name, this->name, strlen (this->name) + 1);

    return has_string_init (name);
}
