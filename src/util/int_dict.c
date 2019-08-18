#include <util/int_dict.h>

struct int_dict_state * int_dict_init () {
    struct int_dict_state * state;

    state         = (struct int_dict_state *)calloc (1, sizeof (struct int_dict_state));
    state->keys   = (int *)calloc (1, INT_DICT_SIZE);
    state->vals   = (int *)calloc (1, INT_DICT_SIZE);
    state->length = 0;
    state->size   = INT_DICT_SIZE;

    return state;
}

void int_dict_free (struct int_dict_state * state) {
    free (state->keys);
    free (state->vals);

    free (state);
}

static int get_key_index (struct int_dict_state * state, int key);
static void reallocate   (struct int_dict_state * state);

int int_dict_get (struct int_dict_state * state, int key) {
    int index = get_key_index (state, key);

    if (index == -1) {
        return -1;
    }

    return state->vals [index];
}

void int_dict_set (struct int_dict_state * state, int key, int val) {
    int index = get_key_index (state, key);

    if (index != -1) {
        state->vals [index] = val;
    } else {
        while (state->length >= state->size) {
            reallocate (state);
        }

        state->keys [state->length] = key;
        state->vals [state->length] = val;
        state->length++;
    }
}

static int get_key_index (struct int_dict_state * state, int key) {
    for (int i = 0; i < state->length; i++) {
        if (state->keys [i] == key) {
            return i;
        }
    }

    return -1;
}

static void reallocate (struct int_dict_state * state) {
    state->size += INT_DICT_SIZE;
    state->keys = realloc (state->keys, state->size);
    state->vals = realloc (state->keys, state->size);
}
