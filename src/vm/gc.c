#include <vm/gc.h>

static struct vector_state * objects;

void gc_init () {
    objects = vector_init ();
}

void gc_free () {
    vector_free (objects);
}

void gc_add (struct has_obj * obj) {
    vector_push (objects, obj);
}

void gc_add_ref (struct has_obj * obj) {
    obj->ref_count++;
}

void gc_remove_ref (struct has_obj * obj) {
    obj->ref_count--;
}

void gc_run () {
    struct vector_state * new_objects;
    struct has_obj      * obj;

    new_objects = vector_init ();

    for (int i = 0; i < objects->length; i++) {
        obj = vector_get (objects, i);

        if (obj->ref_count > 0) {
            vector_push  (new_objects, obj);
        } else {
            has_obj_free (obj);
        }
    }
    
    vector_free (objects);
    objects = new_objects;
}
