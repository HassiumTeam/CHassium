#include <has_lib/has_obj.h>

struct has_obj * has_obj_init (void * state) {
    struct has_obj * obj;

    obj = (struct has_obj *)calloc (1, sizeof (struct has_obj));
    obj->attribs = dict_init ();
    obj->state   = state;
}

void has_obj_free (struct has_obj * obj) {
    dict_free (obj->attribs);
    if (obj->state) {
        free (obj->state);
    }
    free (obj);
}

struct has_obj * has_obj_get_attrib (struct has_obj * obj, char * name) {
    return (struct has_obj *)dict_get (obj->attribs, name);
}

void has_obj_set_attrib (struct has_obj * obj, char * name, struct has_obj * val) {
    dict_set (obj->attribs, name, val);
}
