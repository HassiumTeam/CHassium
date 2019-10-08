#include <emit/emit.h>

struct emit * emit_init () {
    struct emit * emit;

    emit               = (struct emit *)calloc (1, sizeof (struct emit));
    emit->symbol_table = symbol_table_init ();

    return emit;
}

void free_emit (struct emit * emit) {
    free_symbol_table (emit->symbol_table);
    free              (emit);
}

static void accept               (struct emit * emit, struct ast_node * node);
static void accept_assign        (struct emit * emit, struct ast_node * node);
static void accept_attrib_access (struct emit * emit, struct ast_node * node);
static void accept_bin_op        (struct emit * emit, struct ast_node * node);
static void accept_block         (struct emit * emit, struct ast_node * node);
static void accept_break         (struct emit * emit, struct ast_node * node);
static void accept_class         (struct emit * emit, struct ast_node * node);
static void accept_closure       (struct emit * emit, struct ast_node * node);
static void accept_continue      (struct emit * emit, struct ast_node * node);
static void accept_expr_stmt     (struct emit * emit, struct ast_node * node);
static void accept_for           (struct emit * emit, struct ast_node * node);
static void accept_foreach       (struct emit * emit, struct ast_node * node);
static void accept_func_call     (struct emit * emit, struct ast_node * node);
static void accept_func_decl     (struct emit * emit, struct ast_node * node);
static void accept_id            (struct emit * emit, struct ast_node * node);
static void accept_if            (struct emit * emit, struct ast_node * node);
static void accept_import        (struct emit * emit, struct ast_node * node);
static void accept_list_decl     (struct emit * emit, struct ast_node * node);
static void accept_number        (struct emit * emit, struct ast_node * node);
static void accept_obj_decl      (struct emit * emit, struct ast_node * node);
static void accept_raise         (struct emit * emit, struct ast_node * node);
static void accept_return        (struct emit * emit, struct ast_node * node);
static void accept_string        (struct emit * emit, struct ast_node * node);
static void accept_subscript     (struct emit * emit, struct ast_node * node);
static void accept_super         (struct emit * emit, struct ast_node * node);
static void accept_try_catch     (struct emit * emit, struct ast_node * node);
static void accept_typeof        (struct emit * emit, struct ast_node * node);
static void accept_unary_op      (struct emit * emit, struct ast_node * node);
static void accept_use           (struct emit * emit, struct ast_node * node);
static void accept_while         (struct emit * emit, struct ast_node * node);

static void accept (struct emit * emit, struct ast_node * node) {
    switch (node->type) {
        case ASSIGN_NODE:
            accept_assign        (emit, node);
            break;
        case ATTRIB_ACCESS_NODE:
            accept_attrib_access (emit, node);
            break;
        case BIN_OP_NODE:
            accept_bin_op        (emit, node);
            break;
        case BLOCK_NODE:
            accept_block         (emit, node);
            break;
        case BREAK_NODE:
            accept_break         (emit, node);
            break;
        case CLASS_NODE:
            accept_class         (emit, node);
            break;
        case CLOSURE_NODE:
            accept_closure       (emit, node);
            break;
        case CONTINUE_NODE:
            accept_continue      (emit, node);
            break;
        case EXPR_STMT_NODE:
            accept_expr_stmt     (emit, node);
            break;
        case FOR_NODE:
            accept_for           (emit, node);
            break;
        case FOREACH_NODE:
            accept_foreach       (emit, node);
            break;
        case FUNC_CALL_NODE:
            accept_func_call     (emit, node);
            break;
        case FUNC_DECL_NODE:
            accept_func_decl     (emit, node);
            break;
        case ID_NODE:
            accept_id            (emit, node);
            break;
        case IF_NODE:
            accept_if            (emit, node);
            break;
        case IMPORT_NODE:
            accept_import        (emit, node);
            break;
        case LIST_DECL_NODE:
            accept_list_decl     (emit, node);
            break;
        case NUMBER_NODE:
            accept_number        (emit, node);
            break;
        case OBJ_DECL_NODE:
            accept_obj_decl      (emit, node);
            break;
        case RAISE_NODE:
            accept_raise         (emit, node);
            break;
        case RETURN_NODE:
            accept_return        (emit, node);
            break;
        case STRING_NODE:
            accept_string        (emit, node);
            break;
        case SUBSCRIPT_NODE:
            accept_subscript     (emit, node);
            break;
        case SUPER_NODE:
            accept_super         (emit, node);
            break;
        case TRY_CATCH_NODE:
            accept_try_catch     (emit, node);
            break;
        case TYPEOF_NODE:
            accept_typeof        (emit, node);
            break;
        case UNARY_OP_NODE:
            accept_unary_op      (emit, node);
            break;
        case USE_NODE:
            accept_use           (emit, node);
            break;
        case WHILE_NODE:
            accept_while         (emit, node);
            break;
    }
}

/*
ASSIGN_NODE,  ATTRIB_ACCESS_NODE, BIN_OP_NODE,      BLOCK_NODE,     BREAK_NODE,
CLASS_NODE,   CLOSURE_NODE,       CONTINUE_NODE,    EXPR_STMT_NODE, FOR_NODE,
FOREACH_NODE, FUNC_CALL_NODE,     FUNC_DECL_NODE,   ID_NODE,        IF_NODE,
IMPORT_NODE,  LIST_DECL_NODE,     NUMBER_NODE,      OBJ_DECL_NODE,  RAISE_NODE,
RETURN_NODE,  STRING_NODE,        SUBSCRIPT_NODE,   SUPER_NODE,     TRY_CATCH_NODE,
TYPEOF_NODE,  UNARY_OP_NODE,      USE_NODE,         WHILE_NODE,
*/

static void accept_assign (struct emit * emit, struct ast_node * node) {}

static void accept_attrib_access (struct emit * emit, struct ast_node * node) {}

static void accept_bin_op        (struct emit * emit, struct ast_node * node) {}

static void accept_block         (struct emit * emit, struct ast_node * node) {}

static void accept_break         (struct emit * emit, struct ast_node * node) {}

static void accept_class         (struct emit * emit, struct ast_node * node) {}

static void accept_closure       (struct emit * emit, struct ast_node * node) {}

static void accept_continue      (struct emit * emit, struct ast_node * node) {}

static void accept_expr_stmt     (struct emit * emit, struct ast_node * node) {}

static void accept_for           (struct emit * emit, struct ast_node * node) {}

static void accept_foreach (struct emit * emit, struct ast_node * node) {

}

static void accept_func_call (struct emit * emit, struct ast_node * node) {

}

static void accept_func_decl (struct emit * emit, struct ast_node * node) {

}

static void accept_id (struct emit * emit, struct ast_node * node) {}

static void accept_if (struct emit * emit, struct ast_node * node) {}

static void accept_import        (struct emit * emit, struct ast_node * node) {}

static void accept_list_decl     (struct emit * emit, struct ast_node * node) {}

static void accept_number        (struct emit * emit, struct ast_node * node) {}

static void accept_obj_decl      (struct emit * emit, struct ast_node * node) {}

static void accept_raise         (struct emit * emit, struct ast_node * node) {}

static void accept_return        (struct emit * emit, struct ast_node * node) {}

static void accept_string        (struct emit * emit, struct ast_node * node) {}

static void accept_subscript     (struct emit * emit, struct ast_node * node) {}

static void accept_super         (struct emit * emit, struct ast_node * node) {}

static void accept_try_catch     (struct emit * emit, struct ast_node * node) {}

static void accept_typeof        (struct emit * emit, struct ast_node * node) {}

static void accept_unary_op      (struct emit * emit, struct ast_node * node) {}

static void accept_use           (struct emit * emit, struct ast_node * node) {}

static void accept_while         (struct emit * emit, struct ast_node * node) {}
