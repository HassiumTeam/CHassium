#include <emit/symbol_table.h>

static struct scope * scope_init ();
static void           free_scope (struct scope * scope);

static void scope_reallocate (struct scope * scope);
static int  scope_get        (struct scope * scope, char * id);
static int  scope_get_index  (struct scope * scope, char * id);
static void scope_set        (struct scope * scope, char * id, int val);

struct symbol_table * symbol_table_init () {
    struct symbol_table * table;

    table         = (struct symbol_table *)calloc (1, sizeof (struct symbol_table));
    table->next   = 0;
    table->scopes = vector_init ();

    return table;
}

void free_symbol_table (struct symbol_table * table) {
    for (int i = 0; i < table->scopes->length; i++) {
        free_scope (vector_get (table->scopes, i));
    }
    free_vector (table->scopes);

    free (table);
}

void enter_scope (struct symbol_table * table) {
    vector_push (table->scopes, scope_init ());
}

int get_symbol (struct symbol_table * table, char * id) {
    int val;

    for (int i = 0; i < table->scopes->length; i++) {
        val = scope_get (vector_get (table->scopes, i), id);

        if (val != -1) {
            return val;
        }
    }

    return -1;
}

int handle_symbol (struct symbol_table * table, char * id) {
    if (!has_symbol (table, id)) {
        scope_set (vector_peek (table->scopes), id, table->next++);
    }

    return get_symbol (table, id);
}

int has_symbol (struct symbol_table * table, char * id) {
    int val;

    for (int i = 0; i < table->scopes->length; i++) {
        val = scope_get (vector_get (table->scopes, i), id);

        if (val != -1) {
            return val;
        }
    }

    return -1;
}

int in_global_scope (struct symbol_table * table) {
    return table->scopes->length == 1;
}

void leave_scope (struct symbol_table * table) {
    free_scope (vector_pop (table->scopes));
}

int tmp_symbol (struct symbol_table * table) {
    return table->next++;
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

static void free_scope (struct scope * scope) {
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
        scope_reallocate (scope);
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

static void scope_reallocate (struct scope * scope) {
    scope->size += SCOPE_SIZE;

    scope->ids  = realloc (scope->ids,  sizeof (char *) * scope->size);
    scope->vals = realloc (scope->vals, sizeof (int)    * scope->size);
}
