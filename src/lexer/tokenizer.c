#include <lexer/tokenizer.h>

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

static void next_id     (struct tokenizer_state * state, struct token * token);
static void next_number (struct tokenizer_state * state, struct token * token);
static void next_string (struct tokenizer_state * state, struct token * token);
static void whitespace  (struct tokenizer_state * state);
static int  nextc       (struct tokenizer_state * state);
static int  readc       (struct tokenizer_state * state);
static int  peekc       (struct tokenizer_state * state);

void tokenizer_next (struct tokenizer_state * state, struct token * token) {
    char cur, next;

    token->type = unknown;
    token->val = 0;

    whitespace (state);

    if (peekc (state) == -1) {
        token->type = eof;
        return;
    }

    cur = (char)peekc (state);
    next = (char)nextc (state);


    if (isdigit (cur)) {
        next_number (state, token);
    } else if (isalpha (cur) || cur == '_') {
        next_id (state, token);
    } else {
        readc (state); // read over cur

        switch (cur) {
            case '"':
                next_string (state, token);
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
                if (next == '=') {
                    token->type = assign;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else {
                    token->type = op;
                    token->val = (char *)malloc (2);
                    token->val [0] = cur;
                    token->val [1] = 0;
                }
                break;
            case '=':
                if (next == '=') {
                    token->type = comp;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else {
                    token->type = assign;
                    token->val = (char *)malloc (2);
                    token->val [0] = cur;
                    token->val [1] = 0;
                }
                break;
            case '<':
            case '>':
                if (cur == next) {
                    token->type = op;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else if (next == '=') {
                    token->type = comp;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else {
                    token->type = comp;
                    token->val = (char *)malloc (2);
                    token->val [0] = cur;
                    token->val [1] = 0;
                }
                break;
            case '!':
                if (next == '=') {
                    token->type = comp;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else {
                    token->type = op;
                    token->val = (char *)malloc (2);
                    token->val [0] = cur;
                    token->val [1] = 0;
                }
                break;
            case '&':
            case '|':
                if (next == cur) {
                    token->type = op;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else if (next == '=') {
                    token->type = assign;
                    token->val = (char *)malloc (3);
                    token->val [0] = cur;
                    token->val [1] = (char)readc (state);
                    token->val [2] = 0;
                } else {
                    token->type = op;
                    token->val = (char *)malloc (2);
                    token->val [0] = cur;
                    token->val [1] = 0;
                }
                break;
            case '(':
                token->type = oparen;
                break;
            case ')':
                token->type = cparen;
                break;
            case '{':
                token->type = obrace;
                break;
            case '}':
                token->type = cbrace;
                break;
            case '[':
                token->type = osquare;
                break;
            case ']':
                token->type = csquare;
                break;
            case ';':
                token->type = semicol;
                break;
            case '.':
                token->type = dot;
                break;
            case ',':
                token->type = comma;
                break;
            case ':':
                token->type = colon;
                break;
            default:
                printf ("Unknown char \"%c\"\n", cur);
                exit (EXIT_FAILURE);
                break;
        }
    }
}

static void next_id (struct tokenizer_state * state, struct token * token) {
    int size, old_pos, i;

    old_pos = state->pos;
    while (isalnum ((char)peekc (state)) || (char)peekc (state) == '_') { readc (state); }
    size = state->pos - old_pos;
    tokenizer_rewind (state, size);

    token->val = (char *)malloc (size + 1);
    for (i = 0; i < size; i++) {
        token->val [i] = (char)readc (state);
    }
    token->val [i] = 0;

    token->type = id;
}

static void next_number (struct tokenizer_state * state, struct token * token) {
    int size, old_pos, i;

    old_pos = state->pos;
    while (isdigit ((char)peekc (state))) { readc (state); }
    size = state->pos - old_pos;
    tokenizer_rewind (state, size);

    token->val = (char *)malloc (size + 1);
    for (i = 0; i < size; i++) {
        token->val [i] = (char)readc (state);
    }
    token->val [i] = 0;

    token->type = numberc;
}

static void next_string (struct tokenizer_state * state, struct token * token) {
    int size, old_pos, i;

    old_pos = state->pos;
    while ((char)readc (state) != '"') ;
    size = state->pos - old_pos;
    tokenizer_rewind (state, size);

    token->val = (char *)malloc (size);
    for (i = 0; i < size - 1; i ++) {
        token->val [i] = (char)readc (state);
    }
    token->val [i] = 0;

    readc (state); // "

    token->type = stringc;
}

static void whitespace (struct tokenizer_state * state) {
    while (peekc (state) != -1) {
        if (isspace ((char)peekc (state)) == 0) break;
        readc (state);
    }
}

static int nextc (struct tokenizer_state * state) {
    if (state->pos + 1 < state->len) {
        return state->code [state->pos + 1];
    }

    return -1;
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
    state->pos++;
    return -1;
}
