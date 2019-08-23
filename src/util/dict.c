#include <util/dict.h>

static int get_key_index (struct dict_state * state, char * key);

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
    int index;

    index = get_key_index (state, key);

    if (index == -1) {
        return NULL;
    }

    return vector_get (state->vals, index);
}

void dict_set (struct dict_state * state, char * key, void * val) {
    char * new_key;
    int    index;

    new_key = (char *)calloc (strlen (key), strlen (key));
    memcpy (new_key, key, strlen (key));

    index = get_key_index (state, key);

    if (index == -1) {
        vector_push (state->keys, new_key);
        vector_push (state->vals, val);
    } else {
        vector_set  (state->vals, index, val);
    }

    state->count++;
}

int dict_has_key (struct dict_state * state, char * key) {
    return get_key_index (state, key) != -1;
}

static int get_key_index (struct dict_state * state, char * key) {
    char * c;

    for (int i = 0; i < state->keys->length; i++) {
        c = vector_get (state->keys, i);

        if (strcmp (c, key) == 0) {
            return i;
        }
    }

    return -1;
}
