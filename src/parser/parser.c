#include <parser/parser.h>

struct parser_state * parser_init (struct tokenizer_state * lexer) {
    struct parser_state * state = (struct parser_state *)calloc (1, sizeof (struct parser_state));

    state->lexer = lexer;
    state->token = (struct token *)malloc (sizeof (struct token));

    return state;
}

void parser_free (struct parser_state * state) {
    free (state->token);
    free (state);
}

// ----------------------------------------------------------------------------
// Functions for parsing statements.
// ----------------------------------------------------------------------------
static struct ast_node * parse_stmt          (struct parser_state * state);
static struct ast_node * parse_block         (struct parser_state * state);
static struct ast_node * parse_break         (struct parser_state * state);
static struct ast_node * parse_class         (struct parser_state * state);
static struct ast_node * parse_continue      (struct parser_state * state);
static struct ast_node * parse_for           (struct parser_state * state);
static struct ast_node * parse_foreach       (struct parser_state * state);
static struct ast_node * parse_func          (struct parser_state * state);
static struct ast_node * parse_if            (struct parser_state * state);
static struct ast_node * parse_import        (struct parser_state * state);
static struct ast_node * parse_raise         (struct parser_state * state);
static struct ast_node * parse_return        (struct parser_state * state);
static struct ast_node * parse_super         (struct parser_state * state);
static struct ast_node * parse_try_catch     (struct parser_state * state);
static struct ast_node * parse_use           (struct parser_state * state);
static struct ast_node * parse_while         (struct parser_state * state);

// ----------------------------------------------------------------------------
// Miscellaneous parsing functions.
// ----------------------------------------------------------------------------
static struct vector_state * parse_access_chain  (struct parser_state * state);
static struct vector_state * parse_arg_list      (struct parser_state * state);
static struct vector_state * parse_func_args     (struct parser_state * state);
static struct vector_state * parse_object_params (struct parser_state * state);

// ----------------------------------------------------------------------------
// Function for parsing expressions.
// ----------------------------------------------------------------------------
static struct ast_node * parse_expr_stmt  (struct parser_state * state);
static struct ast_node * parse_expr       (struct parser_state * state);
static struct ast_node * parse_assign     (struct parser_state * state);
static struct ast_node * parse_or         (struct parser_state * state);
static struct ast_node * parse_and        (struct parser_state * state);
static struct ast_node * parse_eq         (struct parser_state * state);
static struct ast_node * parse_instanceof (struct parser_state * state);
static struct ast_node * parse_comp       (struct parser_state * state);
static struct ast_node * parse_add        (struct parser_state * state);
static struct ast_node * parse_mult       (struct parser_state * state);
static struct ast_node * parse_unary      (struct parser_state * state);
static struct ast_node * parse_access     (struct parser_state * state);
static struct ast_node * parse_term       (struct parser_state * state);
static struct ast_node * parse_closure    (struct parser_state * state);
static struct ast_node * parse_obj_decl   (struct parser_state * state);
static struct ast_node * parse_typeof     (struct parser_state * state);

// ----------------------------------------------------------------------------
// Functions for navigating the tokens.
// ----------------------------------------------------------------------------
static       int    at_eof     (struct parser_state * state);
static       int    match      (struct parser_state * state, tok_type_t type);
static       int    matchv     (struct parser_state * state, tok_type_t type, const char * val);
static       int    accept     (struct parser_state * state, tok_type_t type);
static       int    acceptv    (struct parser_state * state, tok_type_t type, const char * val);
static       char * expect     (struct parser_state * state, tok_type_t type);
static       char * expectv    (struct parser_state * state, tok_type_t type, const char * val);


struct ast_node * parse (struct parser_state * state) {
    struct vector_state * stmts;

    tokenizer_next (state->lexer, state->token);

    stmts = vector_init ();
    while (!at_eof (state)) {
        vector_push (stmts, parse_stmt (state));
    }

    return block_node_init (stmts);
}

static struct ast_node * parse_stmt (struct parser_state * state) {
    if (match (state, obrace)) {
        return parse_block (state);
    }
}

static struct ast_node * parse_block (struct parser_state * state) {
    struct vector_state * stmts;

    expect (state, obrace);
    while (!match (state, cbrace)) {
        vector_push (stmts, parse_stmt (state));
    }
    expect (state, obrace);

    return block_node_init (stmts);
}

static int at_eof (struct parser_state * state) {
    return state->token->type == eof;
}

static int match (struct parser_state * state, tok_type_t type) {
    if (at_eof (state)) {
        return 0;
    }

    return state->token->type == type;
}

static int matchv (struct parser_state * state, tok_type_t type, const char * val) {
    if (!match (state, type)) {
        return 0;
    }

    return strcmp (state->token->val, val) == 0;
}

static int accept (struct parser_state * state, tok_type_t type) {
    if (match (state, type)) {
        token_free (state->token);
        state->token = token_init ();
        tokenizer_next (state->lexer, state->token);

        return 1;
    }

    return 0;
}

static int acceptv (struct parser_state * state, tok_type_t type, const char * val) {
    if (matchv (state, type, val)) {
        token_free (state->token);
        state->token = token_init ();
        tokenizer_next (state->lexer, state->token);

        return 1;
    }

    return 0;
}

static char * expect (struct parser_state * state, tok_type_t type) {
    if (match (state, type)) {
        char * ret = (char *)malloc (strlen (state->token->val));
        strcpy (ret, state->token->val);

        token_free (state->token);
        state->token = token_init ();
        tokenizer_next (state->lexer, state->token);

        return ret;
    }

    printf ("Expected token with type %d, got %d with value \"%s\"\n",
            type,
            state->token->type,
            state->token->val
    );

    exit (EXIT_FAILURE);

    return 0;
}

static char * expectv (struct parser_state * state, tok_type_t type, const char * val) {
    if (matchv (state, type, val)) {
        char * ret = (char *)malloc (strlen (state->token->val));
        strcpy (ret, state->token->val);

        token_free (state->token);
        state->token = token_init ();
        tokenizer_next (state->lexer, state->token);

        return ret;
    }

    printf ("Expected token with type %d and val \"%s\", got %d with value \"%s\"",
            type,
            val,
            state->token->type,
            state->token->val
    );

    exit (EXIT_FAILURE);

    return 0;
}
