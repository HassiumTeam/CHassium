#include <has_lib/has_class.h>

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_class_init (char * name, struct vector_state * extends) {
    struct has_class * state;
    struct has_obj   * obj;

    state          = (struct has_class *)calloc (1, sizeof (struct has_class));
    state->name    = name;
    state->extends = extends;
    state->type    = has_type_init (name);

    obj = has_obj_init (get_class_type (), state, has_class_free);
    has_obj_set_attrib (obj, "_invoke", has_method_init (obj, _invoke, NULL));

    return obj;
}

void has_class_free (void * state) {
    struct has_class * class = (struct has_class *)state;

    if (class->extends != NULL) {
        for (int i = 0; i < class->extends->length; i++) {
            free (vector_get (class->extends, i));
        }
        vector_free (class->extends);
    }

    free (class->name);

    gc_remove_ref (class->type);

    free (class);
}

static struct has_obj * class_type = NULL;
struct has_obj * get_class_type () {
    if (class_type == NULL) {
        class_type = has_type_init ("class");
    }

    return class_type;
}

static struct has_obj * _invoke (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_class * this;
    struct has_obj   * ret;
    struct has_obj   * _new;

    this = (struct has_class *)self->state;
    ret  = has_class_instantiate (vm, self);

    gc_add_ref (ret);
    _new = has_obj_get_attrib (ret, "new");
    has_obj_invoke (vm, _new, args);
    ret->ref_count--;

    return ret;
}

struct has_obj * has_class_instantiate (struct vm_state * vm, struct has_obj * self) {
    struct has_class  * this;
    struct has_obj    * obj;
    char              * key;
    struct has_obj    * val;

    this = (struct has_class *)self->state;
    obj  = has_obj_init (this->type, NULL, NULL);

    for (int i = 0; i < self->attribs->keys->length; i++) {
        key = vector_get (self->attribs->keys, i);
        val = vector_get (self->attribs->vals, i);

        if (val->user_defined) {
            val = has_bound_func_init (val, obj);
        }

        has_obj_set_attrib (obj, key, val);
    }

    return obj;
}
