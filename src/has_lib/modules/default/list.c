#include <has_lib/modules/default/list.h>

static struct has_obj * _index (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_list_init (struct vector_state * init) {
    struct has_list * state;
    struct has_obj  * obj;

    state = (struct has_list *)calloc (1, sizeof (struct has_list *));

    if (init == NULL) {
        state->vals = vector_init ();
    } else {
        state->vals = init;
    }

    obj = has_obj_init (state, has_list_free);
    has_obj_set_attrib (obj, "_index", has_method_init (obj, _index));

    return obj;
}

void has_list_free (void * state) {
    struct has_list * list = (struct has_list *)state;
    struct has_obj * obj;

    for (int i = 0; i < list->vals->length; i++) {
        obj = vector_get (list->vals, i);
        gc_remove_ref (obj);
    }

    vector_free (list->vals);
    free (list);
}

static struct has_obj * _index (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list   * this;
    struct has_number * num;

    this = (struct has_list *)self->state;
    num = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    vector_free (args);
    
    return vector_get (this->vals, (int)num->val);
}
