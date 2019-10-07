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
static struct vector * parse_func_params   (struct parser * parser);
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
    params = parse_func_params (parser);

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
    char            * id;

    expect_tok_val (parser, ID_TOK, "try");
    try_body   = parse_stmt (parser);

    expect_tok_val (parser, ID_TOK, "catch");

    expect_tok (parser, OPAREN_TOK);
    id = string_copy (expect_tok (parser, ID_TOK)->val);
    expect_tok (parser, CPAREN_TOK);

    catch_body = parse_stmt (parser);

    return ast_node_init (TRY_CATCH_NODE, 0, 0, 3, try_body, catch_body, id);
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
    struct ast_node * left;

    left = parse_instanceof (parser);

    if (accept_tok_val (parser, COMP_TOK, "==")) {
        return ast_node_init (BIN_OP_NODE, EQUAL_BIN_OP, 0, 2,
            left,
            parse_eq (parser)
        );
    } else if (accept_tok_val (parser, COMP_TOK, "!=")) {
        return ast_node_init (UNARY_OP_NODE, LOGICAL_NOT_UNARY_OP, 0, 1,
            ast_node_init (BIN_OP_NODE, EQUAL_BIN_OP, 0, 2,
                left,
                parse_eq (parser)
            )
        );
    }

    return left;
}

static struct ast_node * parse_instanceof (struct parser * parser) {
    struct ast_node * left;

    left = parse_comp (parser);

    if (accept_tok_val (parser, ID_TOK, "instanceof")) {
        return ast_node_init (BIN_OP_NODE, INSTANCEOF_BIN_OP, 0, 2,
            left,
            parse_instanceof (parser)
        );
    }

    return left;
}

static struct ast_node * parse_comp (struct parser * parser) {
    struct ast_node * left;

    left = parse_add (parser);

    if (accept_tok_val (parser, COMP_TOK, ">")) {
        return ast_node_init (BIN_OP_NODE, GREATER_BIN_OP, 0, 2,
            left,
            parse_comp (parser)
        );
    } else if (accept_tok_val (parser, COMP_TOK, ">=")) {
        return ast_node_init (BIN_OP_NODE, GREATER_OR_EQUAL_BIN_OP, 0, 2,
            left,
            parse_comp (parser)
        );
    } else if (accept_tok_val (parser, COMP_TOK, "<")) {
        return ast_node_init (BIN_OP_NODE, LESSER_BIN_OP, 0, 2,
            left,
            parse_comp (parser)
        );
    } else if (accept_tok_val (parser, COMP_TOK, "<=")) {
        return ast_node_init (BIN_OP_NODE, LESSER_OR_EQUAL_BIN_OP, 0, 2,
            left,
            parse_comp (parser)
        );
    }

    return left;
}

static struct ast_node * parse_add (struct parser * parser) {
    struct ast_node * left;

    left = parse_mult (parser);

    if (accept_tok_val (parser, OP_TOK, "+")) {
        return ast_node_init (BIN_OP_NODE, ADD_BIN_OP, 0, 2,
            left,
            parse_add (parser)
        );
    } else if (accept_tok_val (parser, OP_TOK, "-")) {
        return ast_node_init (BIN_OP_NODE, SUB_BIN_OP, 0, 2,
            left,
            parse_add (parser)
        );
    }

    return left;
}

static struct ast_node * parse_mult (struct parser * parser) {
    struct ast_node * left;

    left = parse_unary (parser);

    if (accept_tok_val (parser, OP_TOK, "*")) {
        return ast_node_init (BIN_OP_NODE, MUL_BIN_OP, 0, 2,
            left,
            parse_mult (parser)
        );
    } else if (accept_tok_val (parser, OP_TOK, "/")) {
        return ast_node_init (BIN_OP_NODE, DIV_BIN_OP, 0, 2,
            left,
            parse_mult (parser)
        );
    } else if (accept_tok_val (parser, OP_TOK, "%")) {
        return ast_node_init (BIN_OP_NODE, MOD_BIN_OP, 0, 2,
            left,
            parse_mult (parser)
        );
    }

    return left;
}

static struct ast_node * parse_unary (struct parser * parser) {
    if (accept_tok_val (parser, OP_TOK, "!")) {
        return ast_node_init (UNARY_OP_NODE, LOGICAL_NOT_UNARY_OP, 0, 1,
            parse_unary (parser)
        );
    }

    return parse_access (parser, NULL);
}

static struct ast_node * parse_access (struct parser * parser, struct ast_node * left) {
    struct ast_node * key;

    if (left == NULL) {
        left = parse_term (parser);
    }

    if (accept_tok (parser, DOT_TOK)) {
        return parse_access (parser,
            ast_node_init (ATTRIB_ACCESS_NODE, 0, 0, 2,
                left,
                string_copy (expect_tok (parser, ID_TOK)->val)
            )
        );
    } else if (match_tok (parser, OPAREN_TOK)) {
        return parse_access (parser,
            ast_node_init (FUNC_CALL_NODE, 0, 0, 2,
                left,
                parse_arg_list (parser)
            )
        );
    } else if (accept_tok (parser, OSQUARE_TOK)) {
        key = parse_expr (parser);
        expect_tok (parser, CSQUARE_TOK);

        return parse_access (parser,
            ast_node_init (SUBSCRIPT_NODE, 0, 0, 2,
                left,
                key
            )
        );
    }

    return left;
}

static struct ast_node * parse_term (struct parser * parser) {
    struct ast_node * node;
    struct vector   * elements;
    struct token    * token;

    if (match_tok (parser, NUMBERC_TOK)) {
        return ast_node_init (NUMBER_NODE,
            atof (expect_tok (parser, NUMBERC_TOK)->val),
        0, 0);
    } else if (accept_tok (parser, OPAREN_TOK)) {
        node = parse_expr (parser);
        expect_tok (parser, CPAREN_TOK);

        return node;
    } else if (match_tok_val (parser, ID_TOK, "func")) {
        return parse_closure (parser);
    } else if (accept_tok_val (parser, ID_TOK, "typeof")) {
        expect_tok (parser, OPAREN_TOK);
        node = parse_expr (parser);
        expect_tok (parser, CPAREN_TOK);

        return ast_node_init (TYPEOF_NODE, 0, 0, 1, node);
    } else if (match_tok (parser, ID_TOK)) {
        return ast_node_init (ID_NODE, 0, 0, 1, string_copy (expect_tok (parser, ID_TOK)->val));
    } else if (match_tok (parser, OBRACE_TOK)) {
        return parse_obj_decl (parser);
    } else if (match_tok (parser, OSQUARE_TOK)) {
        elements = vector_init ();

        while (!accept_tok (parser, CSQUARE_TOK)) {
            vector_push (elements, parse_expr (parser));
        }

        return ast_node_init (LIST_DECL_NODE, 0, 0, 1, elements);
    } else if (match_tok (parser, STRINGC_TOK)) {
        return ast_node_init (STRING_NODE, 0, 0, 1, string_copy (expect_tok (parser, STRINGC_TOK)->val));
    } else {
        token = vector_get (parser->tokens, parser->pos);
        printf (
           "Unexpected token with type %d and value \"%s\"!",
           token->type,
           token->val
       );

       exit (EXIT_FAILURE);
    }
}

static struct ast_node * parse_closure (struct parser * parser) {
    struct vector   * params;
    struct vector   * return_type = NULL;
    struct ast_node * body;

    expect_tok_val (parser, ID_TOK, "func");
    params = parse_func_params (parser);

    if (accept_tok (parser, COLON_TOK)) {
        return_type = parse_access_chain (parser);
    }

    body = parse_stmt (parser);

    return ast_node_init (CLOSURE_NODE, 0, 0, 3, params, return_type, body);
}

static struct ast_node * parse_obj_decl (struct parser * parser) {
    struct vector * ids;
    struct vector * vals;
    char          * id;

    ids  = vector_init ();
    vals = vector_init ();

    expect_tok (parser, OBRACE_TOK);

    while (!accept_tok (parser, CBRACE_TOK)) {
        id = string_copy (expect_tok (parser, ID_TOK)->val);
        vector_push (ids, id);

        if (accept_tok (parser, COLON_TOK)) {
            vector_push (vals, parse_expr (parser));
        } else {
            vector_push (vals, ast_node_init (ID_NODE, 0, 0, 1, string_copy (id)));
        }

        accept_tok (parser, COMMA_TOK);
    }

    return ast_node_init (OBJ_DECL_NODE, 0, 0, 2, ids, vals);
}

static struct vector * parse_access_chain (struct parser * parser) {
    struct vector * chain;
    char          * id;

    chain = vector_init ();
    do {
        id = string_copy (expect_tok (parser, ID_TOK)->val);
        vector_push (chain, id);
    } while (accept_tok (parser, DOT_TOK));

    return chain;
}

static struct vector * parse_arg_list (struct parser * parser) {
    struct vector * args;

    expect_tok (parser, OPAREN_TOK);
    args = vector_init ();
    while (!accept_tok (parser, CPAREN_TOK)) {
        vector_push (args, parse_expr (parser));
        accept_tok (parser, COMMA_TOK);
    }

    return args;
}

static struct vector * parse_func_params (struct parser * parser) {
    struct vector     * params;
    struct func_param * param;

    expect_tok (parser, OPAREN_TOK);

    params = vector_init ();
    while (!accept_tok (parser, CPAREN_TOK)) {
        param = (struct func_param *)calloc (1, sizeof (struct func_param));
        if (match_tok (parser, OBRACE_TOK)) {
            param->type = OBJECT_PARAM;
            param->ids  = parse_object_params (parser);
        } else {
            param->type = REGULAR_PARAM;
            param->id   = string_copy (expect_tok (parser, ID_TOK)->val);

            if (accept_tok (parser, COLON_TOK)) {
                param->type          = ENFORCED_PARAM;
                param->enforced_type = parse_access_chain (parser);
            }
        }

        vector_push (params, param);
        accept_tok  (parser, COMMA_TOK);
    }

    return params;
}

static struct vector * parse_object_params (struct parser * parser) {
    struct vector * ids;

    expect_tok (parser, OBRACE_TOK);

    ids = vector_init ();
    do {
        vector_push (ids, string_copy (expect_tok (parser, ID_TOK)->val));
    } while (accept_tok (parser, COMMA_TOK));

    expect_tok (parser, CBRACE_TOK);

    return ids;
}

static int eof (struct parser * parser) {
    return parser->pos >= parser->tokens->length;
}

static int match_tok (struct parser * parser, tok_type_t type) {
    struct token * cur;

    if (eof (parser)) {
        return 0;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type;
}

static int match_tok_val (struct parser * parser, tok_type_t type, char * val) {
    struct token * cur;

    if (eof (parser)) {
        return 0;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type && (strcmp (cur->val, val) == 0);
}

static int accept_tok (struct parser * parser, tok_type_t type) {
    if (match_tok (parser, type)) {
        parser->pos++;

        return 1;
    }

    return 0;
}

static int accept_tok_val (struct parser * parser, tok_type_t type, char * val) {
    if (match_tok_val (parser, type, val)) {
        parser->pos++;

        return 1;
    }

    return 0;
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
