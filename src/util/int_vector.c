#include <util/int_vector.h>

struct int_vector_state * int_vector_init () {
    struct int_vector_state * state;

    state         = (struct int_vector_state *)calloc (1, sizeof (struct int_vector_state));
    state->data   = (int *)calloc (INT_VECTOR_SIZE, sizeof (int));
    state->length = 0;
    state->size   = INT_VECTOR_SIZE;

    return state;
}

void int_vector_free (struct int_vector_state * state) {
    free (state->data);
    free (state);
}

static void reallocate (struct int_vector_state * state);

int int_vector_get (struct int_vector_state * state, int index) {
    return state->data [index];
}

int int_vector_peek (struct int_vector_state * state) {
    return state->data [state->length - 1];
}

int int_vector_pop (struct int_vector_state * state) {
    return state->data [--state->length];
}

void int_vector_push (struct int_vector_state * state, int i) {
    while (state->length >= state->size) {
        reallocate (state);
    }

    state->data [state->length++] = i;
}

static void reallocate (struct int_vector_state * state) {
    state->size += INT_VECTOR_SIZE;

    state->data = realloc (state->data, sizeof (int) * state->size);
}
