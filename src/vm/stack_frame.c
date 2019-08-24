#include <vm/stack_frame.h>

static struct frame * frame_init ();
static void           frame_free (struct frame * frame);

static struct has_obj * frame_get       (struct frame * frame, int id);
static int              frame_get_index (struct frame * frame, int id);
static void             frame_set       (struct frame * frame, int id, struct has_obj * val);


struct stack_frame * stack_frame_init () {
    struct stack_frame * state;

    state               = (struct stack_frame *)calloc (1, sizeof (struct stack_frame));
    state->frames       = vector_init ();
    state->global_frame = frame_init ();

    vector_push (state->frames, state->global_frame);

    return state;
}

void stack_frame_free (struct stack_frame * state) {
    for (int i = 0; i < state->frames->length; i++) {
        frame_free (vector_get (state->frames, i));
    }
    
    vector_free (state->frames);
    free (state);
}

struct has_obj * get_global (struct stack_frame * state, int id) {
    return frame_get (state->global_frame, id);
}

struct has_obj * get_var (struct stack_frame * state, int id) {
    return frame_get (vector_peek (state->frames), id);
}

void set_global (struct stack_frame * state, int id, struct has_obj * val) {
    gc_add_ref (val);

    frame_set (state->global_frame, id, val);
}

void set_var (struct stack_frame * state, int id, struct has_obj * val) {
    gc_add_ref (val);

    frame_set (vector_peek (state->frames), id, val);
}

struct frame * peek_frame (struct stack_frame * state) {
    return vector_peek (state->frames);
}

void pop_frame (struct stack_frame * state) {
    frame_free (vector_pop (state->frames));
}

void push_frame (struct stack_frame * state, struct frame * frame) {
    if (frame == NULL) {
        frame = frame_init ();
    }

    vector_push (state->frames, frame);
}

static struct frame * frame_init () {
    struct frame * frame;

    frame       = (struct frame *)calloc (1, sizeof (struct frame));
    frame->ids  = int_vector_init ();
    frame->vals = vector_init ();

    return frame;
}

static void frame_free (struct frame * frame) {
    int_vector_free (frame->ids);

    for (int i = 0; i < frame->vals->length; i++) {
        gc_remove_ref (vector_get (frame->vals, i));
    }
    vector_free     (frame->vals);

    free (frame);
}

static struct has_obj * frame_get (struct frame * frame, int id) {
    int index;

    index = frame_get_index (frame, id);
    if (index != -1) {
        return vector_get (frame->vals, index);
    }

    return NULL;
}

static int frame_get_index (struct frame * frame, int id) {
    for (int i = 0; i < frame->ids->length; i++) {
        if (int_vector_get (frame->ids, i) == id) {
            return i;
        }
    }

    return -1;
}

static void frame_set (struct frame * frame, int id, struct has_obj * val) {
    int index;

    index = frame_get_index (frame, id);
    if (index == -1) {
        int_vector_push (frame->ids,  id);
        vector_push     (frame->vals, val);
    } else {
        vector_set      (frame->vals, index, val);
    }
}
