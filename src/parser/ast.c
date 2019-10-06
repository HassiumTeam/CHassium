#include <parser/ast.h>

struct ast_node * ast_node_init (node_type_t type, float flag1, float flag2, int count, ...) {
    struct vector   * children;
    struct ast_node * node;
    va_list           ap;

    va_start (ap, count);

    children = vector_init ();
    for (int i = 0; i < count; i++) {
        vector_push (children, va_arg (ap, void *));
    }

    node           = (struct ast_node *)calloc (1, sizeof (struct ast_node));
    node->children = children;
    node->flag1    = flag1;
    node->flag2    = flag2;
    node->type     = type;

    return node;
}

static void free_assign_node        (struct ast_node * node);
static void free_attrib_access_node (struct ast_node * node);
static void free_bin_op_node        (struct ast_node * node);
static void free_block_node         (struct ast_node * node);
static void free_class_node         (struct ast_node * node);
static void free_closure_node       (struct ast_node * node);
static void free_expr_stmt_node     (struct ast_node * node);
static void free_for_node           (struct ast_node * node);
static void free_foreach_node       (struct ast_node * node);
static void free_func_call_node     (struct ast_node * node);
static void free_func_decl_node     (struct ast_node * node);
static void free_id_node            (struct ast_node * node);
static void free_if_node            (struct ast_node * node);
static void free_import_node        (struct ast_node * node);
static void free_list_decl_node     (struct ast_node * node);
static void free_number_node        (struct ast_node * node);
static void free_obj_decl_node      (struct ast_node * node);
static void free_raise_node         (struct ast_node * node);
static void free_return_node        (struct ast_node * node);
static void free_string_node        (struct ast_node * node);
static void free_subscript_node     (struct ast_node * node);
static void free_super_node         (struct ast_node * node);
static void free_try_catch_node     (struct ast_node * node);
static void free_typeof_node        (struct ast_node * node);
static void free_unary_op_node      (struct ast_node * node);
static void free_use_node           (struct ast_node * node);
static void free_while_node         (struct ast_node * node);

void free_ast_node (struct ast_node * node) {
    switch (node->type) {
        case ASSIGN_NODE:
            free_assign_node        (node);
            break;
        case ATTRIB_ACCESS_NODE:
            free_attrib_access_node (node);
            break;
        case BIN_OP_NODE:
            free_bin_op_node        (node);
            break;
        case BLOCK_NODE:
            free_block_node         (node);
            break;
        case CLASS_NODE:
            free_class_node         (node);
            break;
        case CLOSURE_NODE:
            free_closure_node       (node);
            break;
        case EXPR_STMT_NODE:
            free_expr_stmt_node     (node);
            break;
        case FOR_NODE:
            free_for_node           (node);
            break;
        case FOREACH_NODE:
            free_foreach_node       (node);
            break;
        case FUNC_CALL_NODE:
            free_func_call_node     (node);
            break;
        case FUNC_DECL_NODE:
            free_func_decl_node     (node);
            break;
        case ID_NODE:
            free_id_node            (node);
            break;
        case IF_NODE:
            free_if_node            (node);
            break;
        case IMPORT_NODE:
            free_import_node        (node);
            break;
        case LIST_DECL_NODE:
            free_list_decl_node     (node);
            break;
        case OBJ_DECL_NODE:
            free_obj_decl_node      (node);
            break;
        case RAISE_NODE:
            free_raise_node         (node);
            break;
        case RETURN_NODE:
            free_return_node        (node);
            break;
        case STRING_NODE:
            free_string_node        (node);
            break;
        case SUBSCRIPT_NODE:
            free_subscript_node     (node);
            break;
        case SUPER_NODE:
            free_super_node         (node);
            break;
        case TRY_CATCH_NODE:
            free_try_catch_node     (node);
            break;
        case TYPEOF_NODE:
            free_typeof_node        (node);
            break;
        case UNARY_OP_NODE:
            free_unary_op_node      (node);
            break;
        case USE_NODE:
            free_use_node           (node);
            break;
        case WHILE_NODE:
            free_while_node         (node);
            break;
    }

    free_vector (node->children);
    free        (node);
}

static void free_assign_node        (struct ast_node * node) {}
static void free_attrib_access_node (struct ast_node * node) {}
static void free_bin_op_node        (struct ast_node * node) {}
static void free_block_node         (struct ast_node * node) {}
static void free_class_node         (struct ast_node * node) {}
static void free_closure_node       (struct ast_node * node) {}
static void free_expr_stmt_node     (struct ast_node * node) {}
static void free_for_node           (struct ast_node * node) {}
static void free_foreach_node       (struct ast_node * node) {}
static void free_func_call_node     (struct ast_node * node) {}
static void free_func_decl_node     (struct ast_node * node) {}
static void free_id_node            (struct ast_node * node) {}
static void free_if_node            (struct ast_node * node) {}
static void free_import_node        (struct ast_node * node) {}
static void free_list_decl_node     (struct ast_node * node) {}
static void free_number_node        (struct ast_node * node) {}
static void free_obj_decl_node      (struct ast_node * node) {}
static void free_raise_node         (struct ast_node * node) {}
static void free_return_node        (struct ast_node * node) {}
static void free_string_node        (struct ast_node * node) {}
static void free_subscript_node     (struct ast_node * node) {}
static void free_super_node         (struct ast_node * node) {}
static void free_try_catch_node     (struct ast_node * node) {}
static void free_typeof_node        (struct ast_node * node) {}
static void free_unary_op_node      (struct ast_node * node) {}
static void free_use_node           (struct ast_node * node) {}
static void free_while_node         (struct ast_node * node) {}
