#include <vector.h>

static void reallocate (struct vector_state * state);

struct vector_state * vector_init () {
    struct vector_state * state = (struct vector_state *)malloc (sizeof (struct vector_state));

    state->length = 0;
    state->size = VECTOR_SIZE;
    state->data = malloc (sizeof (void *) * VECTOR_SIZE);

    return state;
}

void vector_free (struct vector_state * state) {
    free (state->data);
    free (state);
}

void vector_push (struct vector_state * state, void * ptr) {
    state->data [state->length++] = ptr;

    if (state->length >= state->size) {
        reallocate (state);
    }
}

void * vector_pop (struct vector_state * state) {
    return state->data [--state->length];
}

void * vector_peek (struct vector_state * state) {
    return state->data [state->length - 1];
}

void * vector_get (struct vector_state * state, int index) {
    return state->data [index];
}

void vector_set (struct vector_state * state, int index, void * val) {
    while (index >= state->size) {
        reallocate (state);
    }

    state->data [index] = val;
}

static void reallocate (struct vector_state * state) {
    state->size += VECTOR_SIZE;

    state->data = realloc (state->data, sizeof (void *) * state->size);
}
