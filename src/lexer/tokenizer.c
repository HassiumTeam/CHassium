#include <inc/lexer/tokenizer.h>

struct tokenizer_state * tokenizer_init (FILE * f) {
    struct tokenizer_state * state = (struct tokenizer_state *)malloc(sizeof (struct tokenizer_state));

    state->pos = 0;
    fseek (f, 0, SEEK_END);
    state->len = (uint32_t)ftell (f);
    rewind (f);

    int c;
    state->code = (char *)malloc (state->len);
    while ((c = fgetc (f)) != EOF) {
        state->code [state->pos++] = c;
    }

    state->pos = 0;

    state->row = 1;
    state->col = 1;

    return state;
}

void tokenizer_free (struct tokenizer_state * state) {
    if (state->code != NULL) {
        free (state->code);
    }

    free (state);
}

int tokenizer_rewind (struct tokenizer_state * state, int count) {
    if ((state->pos - count) >= 0 && (state->pos - count) < state->len) {
        state->pos -= count;
        return 1;
    }
    return 0;
}

int tokenizer_forward (struct tokenizer_state * state, int count) {
    if ((state->pos + count) >= 0 && (state->pos + count) < state->len) {
        state->pos += count;
        return 1;
    }

    return 0;
}

int get_tokenizer_pos (struct tokenizer_state * state) {
    return state->pos;
}

int set_tokenizer_pos (struct tokenizer_state * state, int pos) {
    if (pos < state->len && pos >= 0) {
        return 1;
    }

    return 0;
}

static void whitespace  (struct tokenizer_state * state);
static void next_id     (struct tokenizer_state * state, struct token * token);
static void next_number (struct tokenizer_state * state, struct token * token);
static void next_string (struct tokenizer_state * state, struct token * token);
static int  readc       (struct tokenizer_state * state);
static int  peekc       (struct tokenizer_state * state);

void tokenizer_next (struct tokenizer_state * state, struct token * token) {
    next_string (state, token);
}

static void next_string (struct tokenizer_state * state, struct token * token) {
    int size, old_pos, i;
    char * str;

    readc (state); // "

    old_pos = state->pos;
    while ((char)readc (state) != '"');
    size = state->pos - old_pos;

    tokenizer_rewind (state, size);
    str = (char *)malloc (size);
    for (i = 0; i < size - 1; i++) {
        str [i] = (char)readc (state);
    }
    str [i] = 0;

    readc (state); // "
    token->type = stringc;
    token->val = str;
}

static void whitespace (struct tokenizer_state * state) {
    while (peekc (state) != -1) {
        if (isspace ((char)peekc (state)) == 0) break;
        readc (state);
    }
}

static int peekc (struct tokenizer_state * state) {
    if (state->pos < state->len) {
        return state->code [state->pos];
    }

    return -1;
}

static int readc (struct tokenizer_state * state) {
    int c;

    if (state->pos + 1 < state->len) {
        c = state->code [state->pos++];
        if (c == '\n') {
            state->row++;
            state->col = 1;
        }

        return c;
    }

    return -1;
}
