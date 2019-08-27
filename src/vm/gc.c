#include <vm/gc.h>

struct has_obj * gc_add_ref (struct has_obj * obj) {
    obj->ref_count++;
    return obj;
}

struct has_obj * gc_remove_ref (struct has_obj * obj) {
    obj->ref_count--;
    if (obj->ref_count <= 0) {
        has_obj_free (obj);
    }
}

struct vector_state * gc_remove_vect (struct vector_state * vect) {
    for (int i = 0; i < vect->length; i++) {
        gc_remove_ref (vector_get (vect, i));
    }
}
