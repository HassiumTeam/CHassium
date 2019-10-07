#include <util/vector.h>

static void reallocate (struct vector * vector);

struct vector * vector_init () {
    struct vector * vector;

    vector         = (struct vector *)calloc (1, sizeof (struct vector));
    vector->data   = calloc (VECTOR_SIZE_INC, sizeof (void *));
    vector->length = 0;
    vector->size   = VECTOR_SIZE_INC;

    return vector;
}

void free_string_vector (struct vector * vector) {
    for (int i = 0; i < vector->length; i++) {
        free (vector_get (vector, i));
    }

    free_vector (vector);
}

void free_vector (struct vector * vector) {
    free (vector->data);
    free (vector);
}

void * vector_get (struct vector * vector, int i) {
    if (i < 0 || i >= vector->length) {
        return NULL;
    }

    return vector->data [i];
}

void * vector_peek (struct vector * vector) {
    return vector->data [vector->length - 1];
}

void * vector_pop (struct vector * vector) {
    if (vector->length == 0) {
        return NULL;
    }

    return vector->data [--vector->length];
}

void vector_push (struct vector * vector, void * ptr) {
    vector->data [vector->length++] = ptr;

    while (vector->length >= vector->size) {
        reallocate (vector);
    }
}

void vector_set (struct vector * vector, int i, void * ptr) {
    while (vector->size <= i) {
        reallocate (vector);
    }

    vector->data [i] = ptr;
}

static void reallocate (struct vector * vector) {
    vector->size += VECTOR_SIZE_INC;

    vector->data = realloc (vector->data, sizeof (void *) * vector->size);
}
