#include <util/dict.h>

struct dict_state * dict_init () {
    struct dict_state * state;

    state        = (struct dict_state *)calloc (1, sizeof (struct dict_state));
    state->keys  = vector_init ();
    state->vals  = vector_init ();
    state->count = 0;
}

void dict_free (struct dict_state * state) {
    for (int i = 0; i < state->keys->length; i++) {
        free (vector_get (state->keys, i));
    }

    vector_free (state->keys);
    vector_free (state->vals);

    free (state);
}

void * dict_get (struct dict_state * state, char * key) {
    char * key_;

    for (int i = 0; i < state->keys->length; i++) {
        key_ = vector_get (state->keys, i);
        if (strcmp (key, key_) == 0) {
            return vector_get (state->vals, i);
        }
    }

    return NULL;
}

void dict_set (struct dict_state * state, char * key, void * val) {
    char * new_key;

    new_key = (char *)calloc (strlen (key), strlen (key));
    memcpy (new_key, key, strlen (key));
    vector_push (state->keys, new_key);
    vector_push (state->vals, val);
    state->count++;
}
