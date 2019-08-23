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

    return state;
}

void stack_frame_free (struct stack_frame * state) {
    for (int i = 0; i < state->frames->length; i++) {
        frame_free (vector_get (state->frames, i));
    }
    vector_free (state->frames);

    frame_free  (state->global_frame);
    free (state);
}

struct has_obj * get_global (struct stack_frame * state, int id) {
    return frame_get (state->global_frame, id);
}

struct has_obj * get_var (struct stack_frame * state, int id) {
    return frame_get (vector_peek (state->frames), id);
}

void set_global (struct stack_frame * state, int id, struct has_obj * val) {
    frame_set (state->global_frame, id, val);
}

void set_var (struct stack_frame * state, int id, struct has_obj * val) {
    frame_set (vector_peek (state->frames), id, val);
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
