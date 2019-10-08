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

static void accept               (struct emit * emit, struct ast_node * node) {}
static void accept_assign        (struct emit * emit, struct ast_node * node) {}
static void accept_attrib_access (struct emit * emit, struct ast_node * node) {}
static void accept_bin_op        (struct emit * emit, struct ast_node * node) {}
static void accept_block         (struct emit * emit, struct ast_node * node) {}
static void accept_break         (struct emit * emit, struct ast_node * node) {}
static void accept_class         (struct emit * emit, struct ast_node * node) {}
static void accept_closure       (struct emit * emit, struct ast_node * node) {}
static void accept_continue      (struct emit * emit, struct ast_node * node) {}
static void accept_expr_stmt     (struct emit * emit, struct ast_node * node) {}
static void accept_for           (struct emit * emit, struct ast_node * node) {}
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
