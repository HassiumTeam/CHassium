#include <has_lib/has_obj.h>

struct has_obj * has_obj_init (struct has_obj * type, void * state, void (* free_state) (void *)) {
    struct has_obj * obj;
    
    obj               = (struct has_obj *)calloc (1, sizeof (struct has_obj));
    obj->attribs      = dict_init ();
    obj->instructions = vector_init ();
    obj->labels       = int_dict_init ();
    obj->type         = type;
    obj->ref_count    = 0;

    obj->state        = state;
    obj->free_state   = free_state;

    return obj;
}

void has_obj_free (struct has_obj * obj) {
    for (int i = 0; i < obj->attribs->vals->length; i++) {
        if (vector_get (obj->attribs->vals, i) != obj) {
            gc_remove_ref (vector_get (obj->attribs->vals, i));
        }
    }

    dict_free (obj->attribs);

    if (obj->state) {
        obj->free_state (obj->state);
    }

    for (int i = 0; i < obj->instructions->length; i++) {
        inst_free (vector_get (obj->instructions, i));
    }

    vector_free   (obj->instructions);
    int_dict_free (obj->labels);

    free (obj);
}

void has_obj_recursive_free (struct has_obj * obj) {
    for (int i = 0; i < obj->attribs->vals->length; i++) {
        has_obj_recursive_free (vector_get (obj->attribs->vals, i));
    }

    dict_free (obj->attribs);

    if (obj->state) {
        obj->free_state (obj->state);
    }

    for (int i = 0; i < obj->instructions->length; i++) {
        inst_free (vector_get (obj->instructions, i));
    }

    vector_free   (obj->instructions);
    int_dict_free (obj->labels);

    free (obj);
}

static struct has_obj * obj_type = NULL;
struct has_obj * get_obj_type () {
    if (obj_type == NULL) {
        obj_type = has_type_init ("object");
    }

    return obj_type;
}

struct has_obj * has_obj_get_attrib (struct has_obj * obj, char * name) {
    return (struct has_obj *)dict_get (obj->attribs, name);
}

int has_obj_has_attrib (struct has_obj * obj, char * name) {
    return dict_has_key (obj->attribs, name);
}

void has_obj_set_attrib (struct has_obj * obj, char * name, struct has_obj * val) {
    if (has_obj_has_attrib (obj, name)) {
        gc_remove_ref (has_obj_get_attrib (obj, name));
    }

    if (obj != val) {
        gc_add_ref (val);
    }

    dict_set (obj->attribs, name, val);
}

void has_obj_emit (struct has_obj * obj, struct inst * inst) {
    vector_push (obj->instructions, inst);
}

void has_obj_emit_label (struct has_obj * obj, int label) {
    int_dict_set (
        obj->labels,
        label,
        obj->instructions->length
    );
}

int has_obj_get_label (struct has_obj * obj, int label) {
    return int_dict_get (obj->labels, label);
}
