#include <parser/parser.h>

struct parser * parser_init (struct vector * tokens) {
    struct parser * parser;

    parser         = (struct parser *)calloc (1, sizeof (struct parser));
    parser->pos    = 0;
    parser->tokens = tokens;

    return parser;
}

void free_parser (struct parser * parser) {
    free (parser);
}

// ----------------------------------------------------------------------------
// Functions for parsing statements.
// ----------------------------------------------------------------------------
static struct ast_node * parse_stmt          (struct parser * parser);
static struct ast_node * parse_block         (struct parser * parser);
static struct ast_node * parse_break         (struct parser * parser);
static struct ast_node * parse_class         (struct parser * parser);
static struct ast_node * parse_continue      (struct parser * parser);
static struct ast_node * parse_for           (struct parser * parser);
static struct ast_node * parse_foreach       (struct parser * parser);
static struct ast_node * parse_func          (struct parser * parser);
static struct ast_node * parse_if            (struct parser * parser);
static struct ast_node * parse_import        (struct parser * parser);
static struct ast_node * parse_raise         (struct parser * parser);
static struct ast_node * parse_return        (struct parser * parser);
static struct ast_node * parse_super         (struct parser * parser);
static struct ast_node * parse_try_catch     (struct parser * parser);
static struct ast_node * parse_use           (struct parser * parser);
static struct ast_node * parse_while         (struct parser * parser);

// ----------------------------------------------------------------------------
// Function for parsing expressions.
// ----------------------------------------------------------------------------
static struct ast_node * parse_expr_stmt   (struct parser * parser);
static struct ast_node * parse_expr        (struct parser * parser);
static struct ast_node * parse_assign      (struct parser * parser);
static struct ast_node * parse_or          (struct parser * parser);
static struct ast_node * parse_and         (struct parser * parser);
static struct ast_node * parse_bitwise_or  (struct parser * parser);
static struct ast_node * parse_xor         (struct parser * parser);
static struct ast_node * parse_bitwise_and (struct parser * parser);
static struct ast_node * parse_eq          (struct parser * parser);
static struct ast_node * parse_instanceof  (struct parser * parser);
static struct ast_node * parse_comp        (struct parser * parser);
static struct ast_node * parse_add         (struct parser * parser);
static struct ast_node * parse_mult        (struct parser * parser);
static struct ast_node * parse_unary       (struct parser * parser);
static struct ast_node * parse_access      (struct parser * parser, struct ast_node * left);
static struct ast_node * parse_term        (struct parser * parser);
static struct ast_node * parse_closure     (struct parser * parser);
static struct ast_node * parse_obj_decl    (struct parser * parser);
static struct ast_node * parse_typeof      (struct parser * parser);

// ----------------------------------------------------------------------------
// Miscellaneous parsing functions.
// ----------------------------------------------------------------------------
static struct vector * parse_access_chain  (struct parser * parser);
static struct vector * parse_arg_list      (struct parser * parser);
static struct vector * parse_func_args     (struct parser * parser);
static struct vector * parse_object_params (struct parser * parser);

static int eof                       (struct parser * parser);
static int match_tok                 (struct parser * parser, tok_type_t type);
static int match_tok_val             (struct parser * parser, tok_type_t type, char * val);
static int accept_tok                (struct parser * parser, tok_type_t type);
static int accept_tok_val            (struct parser * parser, tok_type_t type, char * val);
static struct token * expect_tok     (struct parser * parser, tok_type_t type);
static struct token * expect_tok_val (struct parser * parser, tok_type_t type, char * val);

struct ast_node * parser_run (struct parser * parser) {
    struct vector * stmts;

    stmts = vector_init ();
    while (!eof (parser)) {
        vector_push (stmts, parse_stmt (parser));
    }

    return ast_node_init (BLOCK_NODE, 0, 0, 1, stmts);
}

static struct ast_node * parse_stmt (struct parser * parser) {
    struct ast_node * ret;

    if (match_tok (parser, OBRACE_TOK)) {
        ret = parse_block (parser);
    } else if (match_tok_val (parser, ID_TOK, "break")) {
        ret = parse_break (parser);
    } else if (match_tok_val (parser, ID_TOK, "class")) {
        ret = parse_class (parser);
    } else if (match_tok_val (parser, ID_TOK, "continue")) {
        ret = parse_continue (parser);
    } else if (match_tok_val (parser, ID_TOK, "for")) {
        ret = parse_for (parser);
    } else if (match_tok_val (parser, ID_TOK, "foreach")) {
        ret = parse_foreach (parser);
    } else if (match_tok_val (parser, ID_TOK, "func")) {
        ret = parse_func (parser);
    } else if (match_tok_val (parser, ID_TOK, "if")) {
        ret = parse_if (parser);
    } else if (match_tok_val (parser, ID_TOK, "import")) {
        ret = parse_import (parser);
    } else if (match_tok_val (parser, ID_TOK, "raise")) {
        ret = parse_raise (parser);
    } else if (match_tok_val (parser, ID_TOK, "return")) {
        ret = parse_return (parser);
    } else if (match_tok_val (parser, ID_TOK, "super")) {
        ret = parse_super (parser);
    } else if (match_tok_val (parser, ID_TOK, "try")) {
        ret = parse_try_catch (parser);
    } else if (match_tok_val (parser, ID_TOK, "use")) {
        ret = parse_use (parser);
    } else if (match_tok_val (parser, ID_TOK, "while")) {
        ret = parse_while (parser);
    } else {
        ret = parse_expr_stmt (parser);
    }

    accept_tok (parser, SEMICOL_TOK);

    return ret;
}

static struct ast_node * parse_block (struct parser * parser) {
    struct vector * stmts;

    expect_tok (parser, OBRACE_TOK);
    stmts = vector_init ();
    while (!accept_tok (parser, CBRACE_TOK)) {
        vector_push (stmts, parse_stmt (parser));
    }

    return ast_node_init (BLOCK_NODE, 0, 0, 1, stmts);
}

static struct ast_node * parse_break (struct parser * parser) {
    expect_tok_val (parser, ID_TOK, "break");

    return ast_node_init (BREAK_NODE, 0, 0, 0);
}

static struct ast_node * parse_class (struct parser * parser) {
    char            * name;
    struct vector   * extends = NULL;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "class");
    name = string_copy (expect_tok (parser, ID_TOK)->val);

    if (accept_tok_val (parser, ID_TOK, "extends")) {
        extends = parse_access_chain (parser);
    }

    body = parse_block (parser);

    return ast_node_init (CLASS_NODE, 0, 0, 3, name, extends, body);
}

static struct ast_node * parse_continue (struct parser * parser) {
    expect_tok_val (parser, ID_TOK, "continue");

    return ast_node_init (CONTINUE_NODE, 0, 0, 0);
}

static struct ast_node * parse_for (struct parser * parser) {
    struct ast_node * init_stmt;
    struct ast_node * expr;
    struct ast_node * rep_stmt;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "for");
    expect_tok     (parser, OPAREN_TOK);

    init_stmt = parse_stmt (parser);
    accept_tok (parser, SEMICOL_TOK);
    expr      = parse_expr (parser);
    accept_tok (parser, SEMICOL_TOK);
    rep_stmt  = parse_stmt (parser);

    expect_tok (parser, CPAREN_TOK);

    body = parse_stmt (parser);

    return ast_node_init (FOR_NODE, 0, 0, 4, init_stmt, expr, rep_stmt, body);
}

static struct ast_node * parse_foreach (struct parser * parser) {
    char            * var;
    struct ast_node * expr;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "foreach");
    expect_tok     (parser, OPAREN_TOK);

    var = string_copy (expect_tok (parser, ID_TOK)->val);
    expect_tok_val (parser, ID_TOK, "in");
    expr = parse_expr (parser);

    expect_tok (parser, CPAREN_TOK);

    body = parse_stmt (parser);

    return ast_node_init (FOREACH_NODE, 0, 0, 3, var, expr, body);
}

static struct ast_node * parse_func (struct parser * parser) {
    char            * name;
    struct vector   * params;
    struct vector   * ret_type = NULL;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "func");
    name   = string_copy (expect_tok (parser, ID_TOK)->val);
    params = parse_func_args (parser);

    if (accept_tok (parser, COLON_TOK)) {
        ret_type = parse_access_chain (parser);
    }

    body = parse_stmt (parser);

    return ast_node_init (FUNC_DECL_NODE, 0, 0, 4, name, params, ret_type, body);
}

static struct ast_node * parse_if (struct parser * parser) {
    struct ast_node * expr;
    struct ast_node * if_body;
    struct ast_node * else_body = NULL;

    expect_tok_val (parser, ID_TOK, "if");
    expect_tok     (parser, OPAREN_TOK);
    expr = parse_expr (parser);
    expect_tok (parser, CPAREN_TOK);

    if_body = parse_stmt (parser);

    if (accept_tok_val (parser, ID_TOK, "else")) {
        else_body = parse_stmt (parser);
    }

    return ast_node_init (IF_NODE, 0, 0, 3, expr, if_body, else_body);
}

static struct ast_node * parse_import (struct parser * parser) {
    expect_tok_val (parser, ID_TOK, "import");

    if (match_tok (parser, STRINGC_TOK)) {
        return ast_node_init (IMPORT_NODE, 0, 0, 2, 0, string_copy (expect_tok (parser, STRINGC_TOK)->val));
    } else {
        return ast_node_init (IMPORT_NODE, 0, 0, 2, parse_access_chain (parser), 0);
    }
}

static struct ast_node * parse_raise (struct parser * parser) {
    expect_tok_val (parser, ID_TOK, "raise");

    return ast_node_init (RAISE_NODE, 0, 0, 1, parse_expr (parser));
}

static struct ast_node * parse_return (struct parser * parser) {
    expect_tok_val (parser, ID_TOK, "return");

    return ast_node_init (RETURN_NODE, 0, 0, 1, parse_expr (parser));
}

static struct ast_node * parse_super (struct parser * parser) {
    struct vector * args;

    expect_tok_val (parser, ID_TOK, "super");
    args = parse_arg_list (parser);

    return ast_node_init (SUPER_NODE, 0, 0, 1, args);
}

static struct ast_node * parse_try_catch (struct parser * parser) {
    struct ast_node * try_body;
    struct ast_node * catch_body;

    expect_tok_val (parser, ID_TOK, "try");
    try_body   = parse_stmt (parser);

    expect_tok_val (parser, ID_TOK, "catch");
    catch_body = parse_stmt (parser);

    return ast_node_init (TRY_CATCH_NODE, 0, 0, 2, try_body, catch_body);
}

static struct ast_node * parse_use (struct parser * parser) {
    struct vector   * imports;
    struct ast_node * expr;

    imports = vector_init ();
    do {
        vector_push (imports, string_copy (expect_tok (parser, ID_TOK)->val));
    } while (accept_tok (parser, COMMA_TOK));

    expect_tok_val (parser, ID_TOK, "from");
    expr = parse_expr (parser);

    return ast_node_init (USE_NODE, 0, 0, 2, imports, expr);
}

static struct ast_node * parse_while (struct parser * parser) {
    struct ast_node * expr;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "while");

    expect_tok (parser, OPAREN_TOK);
    expr = parse_expr (parser);
    expect_tok (parser, CPAREN_TOK);

    body = parse_stmt (parser);

    return ast_node_init (WHILE_NODE, 0, 0, 2, expr, body);
}

static struct ast_node * parse_expr_stmt (struct parser * parser) {
    struct ast_node * expr;

    expr = parse_expr (parser);
    accept_tok (parser, SEMICOL_TOK);

    return ast_node_init (EXPR_STMT_NODE, 0, 0, 1, expr);
}

static struct ast_node * parse_expr (struct parser * parser) {
    return parse_assign (parser);
}

static struct ast_node * parse_assign (struct parser * parser) {
    struct ast_node * left;
    char            * op;
    bin_op_type_t     op_type;

    left = parse_or (parser);

    if (match_tok (parser, ASSIGN_TOK)) {
        op = expect_tok (parser, ASSIGN_TOK)->val;

        if (strcmp (op, "=") == 0) {
            return ast_node_init (ASSIGN_NODE, 0, 0, 2, left, parse_assign (parser));
        } else {
            op_type = 0;
            if (strcmp (op, "+=") == 0) {
                op_type = ADD_BIN_OP;
            } else if (strcmp (op, "&=") == 0) {
                op_type = BITWISE_AND_BIN_OP;
            } else if (strcmp (op, "|=") == 0) {
                op_type = BITWISE_OR_BIN_OP;
            } else if (strcmp (op, "/=") == 0) {
                op_type = DIV_BIN_OP;
            } else if (strcmp (op, "%=") == 0) {
                op_type = MOD_BIN_OP;
            } else if (strcmp (op, "*=") == 0) {
                op_type = MUL_BIN_OP;
            } else if (strcmp (op, "-=") == 0) {
                op_type = SUB_BIN_OP;
            } else if (strcmp (op, "^=") == 0) {
                op_type = XOR_BIN_OP;
            }

            return ast_node_init (ASSIGN_NODE, 0, 0, 2,
                left,
                ast_node_init (BIN_OP_NODE, op_type, 0, 2,
                    left,
                    parse_assign (parser))
            );
        }
    }

    return left;
}

static struct ast_node * parse_or (struct parser * parser) {
    struct ast_node * left;

    left = parse_and (parser);

    while (accept_tok_val (parser, OP_TOK, "||")) {
        left = ast_node_init (BIN_OP_NODE, LOGICAL_OR_BIN_OP, 0, 2,
            left,
            parse_or (parser)
        );
    }

    return left;
}

static struct ast_node * parse_and (struct parser * parser) {
    struct ast_node * left;

    left = parse_bitwise_or (parser);

    while (accept_tok_val (parser, OP_TOK, "&&")) {
        left = ast_node_init (BIN_OP_NODE, LOGICAL_AND_BIN_OP, 0, 2,
            left,
            parse_or (parser)
        );
    }

    return left;
}

static struct ast_node * parse_bitwise_or (struct parser * parser) {
    struct ast_node * left;

    left = parse_xor (parser);

    while (accept_tok_val (parser, OP_TOK, "|")) {
        left = ast_node_init (BIN_OP_NODE, BITWISE_OR_BIN_OP, 0, 2,
            left,
            parse_or (parser)
        );
    }

    return left;
}

static struct ast_node * parse_xor (struct parser * parser) {
    struct ast_node * left;

    left = parse_bitwise_and (parser);

    while (accept_tok_val (parser, OP_TOK, "^")) {
        left = ast_node_init (BIN_OP_NODE, XOR_BIN_OP, 0, 2,
            left,
            parse_or (parser)
        );
    }

    return left;
}

static struct ast_node * parse_bitwise_and (struct parser * parser) {
    struct ast_node * left;

    left = parse_eq (parser);

    while (accept_tok_val (parser, OP_TOK, "&")) {
        left = ast_node_init (BIN_OP_NODE, BITWISE_AND_BIN_OP, 0, 2,
            left,
            parse_or (parser)
        );
    }

    return left;
}

static struct ast_node * parse_eq (struct parser * parser) {

}

static struct ast_node * parse_instanceof  (struct parser * parser) {

}

static struct ast_node * parse_comp        (struct parser * parser) {

}

static struct ast_node * parse_add         (struct parser * parser) {

}

static struct ast_node * parse_mult        (struct parser * parser) {

}

static struct ast_node * parse_unary       (struct parser * parser) {

}

static struct ast_node * parse_access      (struct parser * parser, struct ast_node * left) {

}

static struct ast_node * parse_term        (struct parser * parser) {

}

static struct ast_node * parse_closure     (struct parser * parser) {

}

static struct ast_node * parse_obj_decl    (struct parser * parser) {

}

static struct ast_node * parse_typeof      (struct parser * parser) {

}


static struct vector * parse_access_chain (struct parser * parser) {

}

static struct vector * parse_arg_list (struct parser * parser) {

}

static struct vector * parse_func_args (struct parser * parser) {

}

static int eof (struct parser * parser) {
    return parser->pos >= parser->tokens->length;
}

static int match_tok (struct parser * parser, tok_type_t type) {
    struct token * cur;

    if (eof (parser)) {
        return -1;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type;
}

static int match_tok_val (struct parser * parser, tok_type_t type, char * val) {
    struct token * cur;

    if (eof (parser)) {
        return -1;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type && (strcmp (cur->val, val) == 0);
}

static int accept_tok (struct parser * parser, tok_type_t type) {
    if (match_tok (parser, type)) {
        parser->pos++;

        return 1;
    }

    return -1;
}

static int accept_tok_val (struct parser * parser, tok_type_t type, char * val) {
    if (match_tok_val (parser, type, val)) {
        parser->pos++;

        return 1;
    }

    return -1;
}

static struct token * expect_tok (struct parser * parser, tok_type_t type) {
    struct token * cur;

    if (match_tok (parser, type)) {
        return vector_get (parser->tokens, parser->pos++);
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    printf ("Parser error! Expected token with type %d, got %d with value \"%s\"!\n", type, cur->type, cur->val);
}

static struct token * expect_tok_val (struct parser * parser, tok_type_t type, char * val) {
    struct token * cur;

    if (match_tok_val (parser, type, val)) {
        return vector_get (parser->tokens, parser->pos++);
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    printf ("Parser error! Expected token with type %d and value \"%s\", got %d with value \"%s\"!\n", type, val, cur->type, cur->val);
}
