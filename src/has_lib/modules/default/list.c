#include <has_lib/modules/default/list.h>

static struct has_obj * _equal       (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _index       (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _iter        (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * _store_index (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * pop          (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * push         (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
static struct has_obj * size         (struct vm_state * vm, struct has_obj * self, struct vector_state * args);

struct has_obj * has_list_init (struct vector_state * init) {
    struct has_list * state;
    struct has_obj  * obj;

    state = (struct has_list *)calloc (1, sizeof (struct has_list *));

    if (init == NULL) {
        state->vals = vector_init ();
    } else {
        state->vals = init;
    }

    obj = has_obj_init (get_list_type (), state, has_list_free);
    has_obj_set_attrib (obj, "_equal",       has_method_init (obj, _equal,       NULL));
    has_obj_set_attrib (obj, "_index",       has_method_init (obj, _index,       NULL));
    has_obj_set_attrib (obj, "_iter",        has_method_init (obj, _iter,        NULL));
    has_obj_set_attrib (obj, "_store_index", has_method_init (obj, _store_index, NULL));
    has_obj_set_attrib (obj, "pop",          has_method_init (obj, pop,          NULL));
    has_obj_set_attrib (obj, "push",         has_method_init (obj, push,         NULL));
    has_obj_set_attrib (obj, "size",         has_method_init (obj, size,         NULL));

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

static struct has_obj * list_type = NULL;
struct has_obj * get_list_type () {
    if (list_type == NULL) {
        list_type = has_type_init ("list");
    }

    return list_type;
}

static struct has_obj * _equal (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list * this;
    struct has_obj  * arg;
    struct has_list * arg_as_list;

    this        = (struct has_list *)self->state;
    arg         = vector_get (args, 0);
    arg_as_list = (struct has_list *)arg->state;

    if (self == arg) {
        return HAS_TRUE;
    }

    if (this->vals->length != arg_as_list->vals->length) {
        return HAS_FALSE;
    }

    for (int i = 0; i < this->vals->length; i++) {
        if (has_obj_equal (vm, vector_get (this->vals, i), vector_get (arg_as_list->vals, i)) != HAS_TRUE) {
            return HAS_FALSE;
        }
    }

    return HAS_TRUE;
}

static struct has_obj * _index (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list   * this;
    struct has_number * index;

    this  = (struct has_list *)self->state;
    index = (struct has_number *)((struct has_obj *)vector_get (args, 0))->state;

    return vector_get (this->vals, (int)index->val);
}

static struct has_obj * _iter (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list     * this;
    struct vector_state * vals;

    this = (struct has_list *)(self->state);

    vals = vector_init ();
    for (int i = 0; i < this->vals->length; i++) {
        vector_push (vals, vector_get (this->vals, i));
    }

    return has_iter_init (vals);
}

static struct has_obj * _store_index (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list   * this;
    int                 index;
    struct has_obj    * val;

    this  = (struct has_list *)self->state;
    index = (int)((struct has_number *)((struct has_obj *)vector_get (args, 0))->state)->val;
    val   = vector_get (args, 1);

    // Remove reference to prior object.
    if (index < this->vals->length) {
        gc_remove_ref (vector_get (this->vals, index));
    }

    vector_set (this->vals, index, val);

    return val;
}

static struct has_obj * pop (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list * this;
    struct has_obj  * obj;

    this = (struct has_list *)self->state;
    obj  = vector_pop (this->vals);
    obj->ref_count--;

    return obj;
}

static struct has_obj * push (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list * this;
    struct has_obj  * obj;

    this = (struct has_list *)self->state;
    obj  = vector_get (args, 0);

    vector_push (this->vals, gc_add_ref (obj));

    return obj;
}

static struct has_obj * size (struct vm_state * vm, struct has_obj * self, struct vector_state * args) {
    struct has_list * this;

    this = (struct has_list *)self->state;

    return has_number_init (this->vals->length);
}
