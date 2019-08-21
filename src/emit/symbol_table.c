#include <emit/symbol_table.h>

static struct scope {
    char ** ids;
    int   * vals;
    int     length;
    int     size;
};

static struct scope * scope_init ();
static void           scope_free (struct scope * scope);

static int  scope_get       (struct scope * scope, char * id);
static void scope_set       (struct scope * scope, char * id, int val);
static int  scope_get_index (struct scope * scope, char * id);
static void reallocate      (struct scope * scope);

struct symbol_table_state * symbol_table_init () {
    struct symbol_table_state * state;

    state = (struct symbol_table_state *)calloc (1, sizeof (struct symbol_table_state));
    state->scopes     = vector_init ();
    state->next_index = 0;

    vector_push (state->scopes, scope_init ());

    return state;
}

void symbol_table_free (struct symbol_table_state * state) {
    int length = state->scopes->length;

    for (int i = 0; i < length; i++) {
        scope_free (vector_pop (state->scopes));
    }

    free (state);
}

void enter_scope (struct symbol_table_state * state) {
    vector_push (state->scopes, scope_init ());
}

int get_symbol (struct symbol_table_state * state, char * id) {
    struct scope * scope;
    int            val;

    for (int i = 0; i < state->scopes->length; i++) {
        scope = vector_get (state->scopes, i);
        val   = scope_get  (scope, id);

        if (val != -1) {
            return val;
        }
    }

    return -1;
}

int handle_symbol (struct symbol_table_state * state, char * id) {
    struct scope * scope;

    if (!has_symbol (state, id)) {
        scope = vector_peek (state->scopes);
        scope_set (scope, id, state->next_index++);
    }

    return get_symbol (state, id);
}

int has_symbol (struct symbol_table_state * state, char * id) {
    struct scope * scope;
    int            val;

    for (int i = 0; i < state->scopes->length; i++) {
        scope = vector_get (state->scopes, i);

        if (scope_get (scope, id) != -1) {
            return 1;
        }
    }

    return 0;
}

int in_global_scope (struct symbol_table_state * state) {
    return state->scopes->length == 1;
}

void leave_scope (struct symbol_table_state * state) {
    scope_free (vector_pop (state->scopes));
}

int tmp_symbol (struct symbol_table_state * state) {
    return state->next_index++;
}

static struct scope * scope_init () {
    struct scope * scope;

    scope         = (struct scope *)calloc (1,          sizeof (struct scope));
    scope->ids    = (char **)       calloc (SCOPE_SIZE, sizeof (char *));
    scope->vals   = (int *)         calloc (SCOPE_SIZE, sizeof (int));
    scope->length = 0;
    scope->size   = SCOPE_SIZE;

    return scope;
}

static void scope_free (struct scope * scope) {
    int length = scope->length;

    for (int i = 0; i < length; i++) {
        free (scope->ids [i]);
    }

    free (scope->ids);
    free (scope->vals);
    free (scope);
}

static int scope_get (struct scope * scope, char * id) {
    int index;

    index = scope_get_index (scope, id);

    if (index == -1) {
        return -1;
    }

    return scope->vals [index];
}

static void scope_set (struct scope * scope, char * id, int val) {
    while (scope->length >= scope->size) {
        reallocate (scope);
    }

    scope->ids  [scope->length] = id;
    scope->vals [scope->length]   = val;

    scope->length++;
}

static int scope_get_index (struct scope * scope, char * id) {
    char * val;

    for (int i = 0; i < scope->length; i++) {
        val = scope->ids [i];

        if (strcmp (val, id) == 0) {
            return i;
        }
    }

    return -1;
}

static void reallocate (struct scope * scope) {
    scope->size += SCOPE_SIZE;

    scope->ids  = realloc (scope->ids,  sizeof (char *) * scope->size);
    scope->vals = realloc (scope->vals, sizeof (int)    * scope->size);
}
