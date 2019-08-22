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
static struct ast_node * parse_access     (struct parser_state * state, struct ast_node * left);
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


struct ast_node * parser_parse (struct parser_state * state) {
    struct vector_state * stmts;

    tokenizer_next (state->lexer, state->token);

    stmts = vector_init ();
    while (!at_eof (state)) {
        vector_push (stmts, parse_stmt (state));
    }

    return block_node_init (stmts);
}

static struct ast_node * parse_stmt (struct parser_state * state) {
    struct ast_node * ret;

    if (match (state, obrace)) {
        ret = parse_block (state);
    } else if (matchv (state, id, "break")) {
        ret = parse_break (state);
    } else if (matchv (state, id, "class")) {
        ret = parse_class (state);
    } else if (matchv (state, id, "continue")) {
        ret = parse_continue (state);
    } else if (matchv (state, id, "for")) {
        ret = parse_for (state);
    } else if (matchv (state, id, "foreach")) {
        ret = parse_foreach (state);
    } else if (matchv (state, id, "func")) {
        ret = parse_func (state);
    } else if (matchv (state, id, "if")) {
        ret = parse_if (state);
    } else if (matchv (state, id, "import")) {
        ret = parse_import (state);
    } else if (matchv (state, id, "raise")) {
        ret = parse_raise (state);
    } else if (matchv (state, id, "return")) {
        ret = parse_return (state);
    } else if (matchv (state, id, "super")) {
        ret = parse_super (state);
    } else if (matchv (state, id, "try")) {
        ret = parse_try_catch (state);
    } else if (matchv (state, id, "use")) {
        ret = parse_use (state);
    } else if (matchv (state, id, "while")) {
        ret = parse_while (state);
    } else {
        ret = parse_expr_stmt (state);
    }

    accept (state, semicol);

    return ret;
}

static struct ast_node * parse_block (struct parser_state * state) {
    struct vector_state * stmts;

    expect (state, obrace);

    stmts = vector_init ();
    while (!match (state, cbrace)) {
        vector_push (stmts, parse_stmt (state));
    }

    expect (state, cbrace);
    return block_node_init (stmts);
}

static struct ast_node * parse_break (struct parser_state * state) {
    free (expectv (state, id, "break"));

    return break_node_init ();
}

static struct ast_node * parse_class (struct parser_state * state) {
    char                * name;
    struct vector_state * extends = 0;
    struct ast_node     * body;

    free (expectv (state, id, "class"));
    name = expect (state, id);

    if (acceptv (state, id, "extends")) {
        extends = parse_access_chain (state);
    }

    body = parse_stmt (state);

    return class_node_init (name, extends, body);
}

static struct ast_node * parse_continue (struct parser_state * state) {
    free (expectv (state, id, "continue"));

    return continue_node_init ();
}

static struct ast_node * parse_for (struct parser_state * state) {
    struct ast_node * pre_stmt;
    struct ast_node * expr;
    struct ast_node * rep_stmt;
    struct ast_node * body;

    free (expectv (state, id, "for"));

    expect (state, oparen);
    pre_stmt = parse_stmt (state);
    accept (state, semicol);
    expr     = parse_expr (state);
    accept (state, semicol);
    rep_stmt = parse_stmt (state);
    expect (state, cparen);

    body     = parse_stmt (state);

    return for_node_init (pre_stmt, expr, rep_stmt, body);
}

static struct ast_node * parse_foreach (struct parser_state * state) {
    char            * id_;
    struct ast_node * target;
    struct ast_node * body;

    free (expectv (state, id, "foreach"));

    expect (state, oparen);
    id_     = expect (state, id);
    free (expectv (state, id, "in"));
    target = parse_expr (state);
    expect (state, cparen);

    body   = parse_stmt (state);

    return foreach_node_init (id_, target, body);
}

static struct ast_node * parse_func (struct parser_state * state) {
    char                * name;
    struct vector_state * params;
    struct vector_state * return_type = 0;
    struct ast_node     * body;

    free (expectv (state, id, "func"));
    name = expect (state, id);
    params = parse_func_args (state);

    if (accept (state, colon)) {
        return_type = parse_access_chain (state);
    }

    body = parse_stmt (state);

    return func_decl_node_init (name, params, return_type, body);
}

static struct ast_node * parse_if (struct parser_state * state) {
    struct ast_node * expr;
    struct ast_node * if_body;
    struct ast_node * else_body = 0;

    free (expectv (state, id, "if"));

    expect (state, oparen);
    expr    = parse_expr (state);
    expect (state, cparen);

    if_body = parse_stmt (state);
    if (acceptv (state, id, "else")) {
        else_body = parse_stmt (state);
    }

    return if_node_init (expr, if_body, else_body);
}

static struct ast_node * parse_import (struct parser_state * state) {
    char * mod;

    free (expectv (state, id, "import"));

    if (match (state, id)) {
        return import_node_init (NULL, parse_access_chain (state));
    } else {
        return import_node_init (expect (state, stringc), NULL);
    }
}

static struct ast_node * parse_raise (struct parser_state * state) {
    struct ast_node * expr;

    free (expectv (state, id, "raise"));
    expr = parse_expr (state);

    return raise_node_init (expr);
}

static struct ast_node * parse_return (struct parser_state * state) {
    struct ast_node * expr;

    free (expectv (state, id, "return"));
    expr = parse_expr (state);

    return return_node_init (expr);
}

static struct ast_node * parse_super (struct parser_state * state) {
    struct vector_state * args;

    free (expectv (state, id, "super"));

    expect (state, oparen);

    args = vector_init ();
    while (!accept (state, cparen)) {
        vector_push (args, parse_expr (state));
        accept (state, comma);
    }

    return super_node_init (args);
}

static struct ast_node * parse_try_catch (struct parser_state * state) {
    struct ast_node * try_body;
    struct ast_node * catch_body;

    free (expectv (state, id, "try"));
    try_body   = parse_stmt (state);
    free (expectv (state, id, "catch"));
    catch_body = parse_stmt (state);

    return try_catch_node_init (try_body, catch_body);
}

static struct ast_node * parse_use (struct parser_state * state) {
    struct vector_state * imports;
    char                * mod;

    free (expectv (state, id, "use"));

    imports = vector_init ();
    do {
        vector_push (imports, expect (state, id));
    } while (accept (state, comma));

    free (expectv (state, id, "from"));
    mod = expect (state, stringc);

    return use_node_init (imports, mod);
}

static struct ast_node * parse_while (struct parser_state * state) {
    struct ast_node * expr;
    struct ast_node * body;

    free (expectv (state, id, "while"));

    expect (state, oparen);
    expr = parse_expr (state);
    expect (state, cparen);

    body = parse_stmt (state);

    return while_node_init (expr, body);
}

static struct ast_node * parse_expr_stmt (struct parser_state * state) {
    struct ast_node * expr;
    expr = parse_expr (state);

    accept (state, semicol);

    return expr_stmt_node_init (expr);
}

static struct ast_node * parse_expr (struct parser_state * state) {
    return parse_assign (state);
}

static struct ast_node * parse_assign (struct parser_state * state) {
    struct ast_node * left;
    char            * val;

    left = parse_or (state);

    if (match (state, assign)) {
        val = state->token->val;
        if (strcmp (val, "=") == 0) {
            accept (state, assign);
            return assign_node_init (left, parse_assign (state), 1);
        } else {
            bin_op_type_t type = 0;
            if (strcmp (val, "+=") == 0) {
                type = add_bin_op;
            } else if (strcmp (val, "&=") == 0) {
                type = bitwise_and_bin_op;
            } else if (strcmp (val, "|=") == 0) {
                type = bitwise_or_bin_op;
            } else if (strcmp (val, "/=") == 0) {
                type = div_bin_op;
            } else if (strcmp (val, "%=") == 0) {
                type = mod_bin_op;
            } else if (strcmp (val, "*=") == 0) {
                type = mul_bin_op;
            } else if (strcmp (val, "-=") == 0) {
                type = sub_bin_op;
            } else if (strcmp (val, "^=") == 0) {
                type = xor_bin_op;
            }
            accept (state, assign);

            return assign_node_init (
                left,
                bin_op_node_init (
                    type,
                    left,
                    parse_assign (state)
                ),
                0
            );
        }
    }

    return left;
}

static struct ast_node * parse_or (struct parser_state * state) {
    struct ast_node * left;

    left = parse_and (state);

    while (acceptv (state, op, "||")) {
        left = bin_op_node_init (
            logical_or_bin_op,
            left,
            parse_or (state)
        );
    }

    return left;
}

static struct ast_node * parse_and (struct parser_state * state) {
    struct ast_node * left;

    left = parse_eq (state);

    while (acceptv (state, op, "&&")) {
        left = bin_op_node_init (
            logical_and_bin_op,
            left,
            parse_and (state)
        );
    }

    return left;
}

static struct ast_node * parse_eq (struct parser_state * state) {
    struct ast_node * left;
    char            * val;

    left = parse_instanceof (state);

    if (match (state, comp)) {
        val = state->token->val;
        if (strcmp (val, "==") == 0) {
            accept (state, comp);
            return bin_op_node_init (
                equal_bin_op,
                left,
                parse_eq (state)
            );
        } else if (strcmp (val, "!=") == 0) {
            accept (state, comp);
            return unary_op_node_init (
                logical_not_unary_op,
                bin_op_node_init (
                    equal_bin_op,
                    left,
                    parse_eq (state)
                )
            );
        }
    }

    return left;
}

static struct ast_node * parse_instanceof (struct parser_state * state) {
    struct ast_node * left;

    left = parse_comp (state);

    if (acceptv (state, id, "instanceof")) {
        return bin_op_node_init (
            instanceof_bin_op,
            left,
            parse_instanceof (state)
        );
    }

    return left;
}

static struct ast_node * parse_comp (struct parser_state * state) {
    struct ast_node * left;
    char            * val;

    left = parse_add (state);

    if (match (state, comp)) {
        val = state->token->val;
        if (strcmp (val, ">") == 0) {
            accept (state, comp);
            return bin_op_node_init (
                greater_bin_op,
                left,
                parse_comp (state)
            );
        } else if (strcmp (val, ">=") == 0) {
            accept (state, comp);
            return bin_op_node_init (
                greater_or_equal_bin_op,
                left,
                parse_comp (state)
            );
        } else if (strcmp (val, "<") == 0) {
            accept (state, comp);
            return bin_op_node_init (
                lesser_bin_op,
                left,
                parse_comp (state)
            );
        } else if (strcmp (val, "<=") == 0) {
            accept (state, comp);
            return bin_op_node_init (
                lesser_or_equal_bin_op,
                left,
                parse_comp (state)
            );
        }
    }

    return left;
}

static struct ast_node * parse_add (struct parser_state * state) {
    struct ast_node * left;
    char            * val;

    left = parse_mult (state);

    if (match (state, op)) {
        val = state->token->val;
        if (strcmp (val, "+") == 0) {
            accept (state, op);
            return bin_op_node_init (
                add_bin_op,
                left,
                parse_add (state)
            );
        } else if (strcmp (val, "-") == 0) {
            accept (state, op);
            return bin_op_node_init (
                sub_bin_op,
                left,
                parse_add (state)
            );
        }
    }

    return left;
}

static struct ast_node * parse_mult (struct parser_state * state) {
    struct ast_node * left;
    char            * val;

    left = parse_unary (state);

    if (match (state, op)) {
        val = state->token->val;
        if (strcmp (val, "%") == 0) {
            accept (state, op);
            return bin_op_node_init (
                mod_bin_op,
                left,
                parse_mult (state)
            );
        } else if (strcmp (val, "*") == 0) {
            accept (state, op);
            return bin_op_node_init (
                mul_bin_op,
                left,
                parse_mult (state)
            );
        } else if (strcmp (val, "/") == 0) {
            accept (state, op);
            return bin_op_node_init (
                div_bin_op,
                left,
                parse_mult (state)
            );
        }
    }

    return left;
}

static struct ast_node * parse_unary (struct parser_state * state) {
    char * val;

    if (match (state, op)) {
        val = state->token->val;
        if (strcmp (val, "!") == 0) {
            return unary_op_node_init (
                logical_not_unary_op,
                parse_unary (state)
            );
        }
    }

    return parse_access (state, NULL);
}

static struct ast_node * parse_access (struct parser_state * state, struct ast_node * left) {
    struct ast_node * key;

    if (left == NULL) {
        left = parse_term (state);
    }

    if (accept (state, dot)) {
        return parse_access (
            state,
            attrib_access_node_init (
                left,
                expect (state, id)
            )
        );
    } else if (match (state, oparen)) {
        return parse_access (
            state,
            func_call_node_init (
                left,
                parse_arg_list (state)
            )
        );
    } else if (accept (state, osquare)) {
        key = parse_expr (state);
        expect (state, csquare);
        return subscript_node_init (
            left,
            key
        );
    }

    return left;
}

static struct ast_node * parse_term (struct parser_state * state) {
    struct ast_node     * expr;
    struct vector_state * elements;
    char                * val;
    float                 f;

    if (match (state, numberc)) {
        val = expect (state, numberc);
        f = atof (val);
        free (val);
        return number_node_init (2);
    } else if (accept (state, oparen)) {
        expr = parse_expr (state);
        expect (state, cparen);

        return expr;
    } else if (matchv (state, id, "func")) {
        return parse_closure (state);
    } else if (acceptv (state, id, "typeof")) {
        expect (state, oparen);
        expr = parse_expr (state);
        expect (state, cparen);

        return typeof_node_init (expr);
    } else if (match (state, id)) {
        return id_node_init (expect (state, id));
    } else if (match (state, obrace)) {
        return parse_obj_decl (state);
    } else if (accept (state, osquare)) {
        elements = vector_init ();
        while (!accept (state, csquare)) {
            vector_push (elements, parse_expr (state));
            accept (state, comma);
        }

        return list_decl_node_init (elements);
    } else if (match (state, stringc)) {
        return string_node_init (expect (state, stringc));
    } else {
        printf (
            "Unexpected token with type %d and value \"%s\"!",
            state->token->type,
            state->token->val
        );

        exit (EXIT_FAILURE);
    }
}

static struct vector_state * parse_access_chain (struct parser_state * state) {
    struct vector_state * chain;

    chain = vector_init ();
    do {
        vector_push (chain, expect (state, id));
    } while (accept (state, dot));

    return chain;
}

static struct vector_state * parse_arg_list (struct parser_state * state) {
    struct vector_state * args;

    expect (state, oparen);

    args = vector_init ();
    while (!accept (state, cparen)) {
        vector_push (args, parse_expr (state));
        accept (state, comma);
    }

    return args;
}

static struct ast_node * parse_closure (struct parser_state * state) {
    struct vector_state * params;
    struct vector_state * return_type = 0;
    struct ast_node     * body;

    free (expectv (state, id, "func"));
    params = parse_func_args (state);

    if (accept (state, colon)) {
        return_type = parse_access_chain (state);
    }

    body   = parse_stmt (state);

    return closure_node_init (
        params,
        return_type,
        body
    );
}

static struct vector_state * parse_func_args (struct parser_state * state) {
    struct vector_state * args;

    args = vector_init ();

    struct func_param * param;
    expect (state, oparen);
    while (!accept (state, cparen)) {
        param = (struct func_param *)calloc (1, sizeof (struct func_param));
        if (match (state, obrace)) {
            param->type = object_param;
            param->vals = parse_object_params (state);
        } else {
            param->id = expect (state, id);
            if (accept (state, colon)) {
                param->enforced_type = parse_access_chain (state);
            }
        }
        vector_push (args, param);
        accept (state, comma);
    }

    return args;
}

static struct ast_node * parse_obj_decl (struct parser_state * state) {
    struct vector_state * ids;
    struct vector_state * vals;
    char                * id_;
    char                * tmp;

    ids = vector_init ();
    vals = vector_init ();

    expect (state, obrace);
    while (!accept (state, cbrace)) {
        id_ = expect (state, id);
        vector_push (ids, id_);

        if (accept (state, colon)) {
            vector_push (vals, parse_expr (state));
        } else {
            tmp = (char *)calloc (strlen (id_) + 1, sizeof (char));
            memcpy (tmp, id_, strlen (id_) + 1);
            vector_push (vals, id_node_init (tmp));
        }

        accept (state, comma);
    }

    return obj_decl_node_init (ids, vals);
}

static struct vector_state * parse_object_params (struct parser_state * state) {
    struct vector_state * ids;

    expect (state, obrace);
    ids = vector_init ();
    while (!accept (state, cbrace)) {
        vector_push (ids, expect (state, id));
        accept (state, comma);
    }

    return ids;
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
    char * ret;

    if (match (state, type)) {
        ret = NULL;
        if (state->token->val) {
            ret = (char *)calloc (strlen (state->token->val) + 1, sizeof (char));
            memcpy (ret, state->token->val, strlen (state->token->val) + 1);
        }

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
    char * ret;
    if (matchv (state, type, val)) {
        ret = NULL;
        if (state->token->val) {
            ret = (char *)malloc (strlen (state->token->val) + 1);
            memcpy (ret, state->token->val, strlen (state->token->val) + 1);
        }

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
