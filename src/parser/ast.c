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

static void free_assign_node        (struct ast_node   * node);
static void free_attrib_access_node (struct ast_node   * node);
static void free_bin_op_node        (struct ast_node   * node);
static void free_block_node         (struct ast_node   * node);
static void free_class_node         (struct ast_node   * node);
static void free_closure_node       (struct ast_node   * node);
static void free_expr_stmt_node     (struct ast_node   * node);
static void free_for_node           (struct ast_node   * node);
static void free_foreach_node       (struct ast_node   * node);
static void free_func_call_node     (struct ast_node   * node);
static void free_func_decl_node     (struct ast_node   * node);
static void free_func_param         (struct func_param * param);
static void free_id_node            (struct ast_node   * node);
static void free_if_node            (struct ast_node   * node);
static void free_import_node        (struct ast_node   * node);
static void free_list_decl_node     (struct ast_node   * node);
static void free_obj_decl_node      (struct ast_node   * node);
static void free_raise_node         (struct ast_node   * node);
static void free_return_node        (struct ast_node   * node);
static void free_string_node        (struct ast_node   * node);
static void free_subscript_node     (struct ast_node   * node);
static void free_super_node         (struct ast_node   * node);
static void free_try_catch_node     (struct ast_node   * node);
static void free_typeof_node        (struct ast_node   * node);
static void free_unary_op_node      (struct ast_node   * node);
static void free_use_node           (struct ast_node   * node);
static void free_while_node         (struct ast_node   * node);

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

static void free_assign_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
}

static void free_attrib_access_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free          (vector_get (node->children, 1));
}

static void free_bin_op_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
}

static void free_block_node (struct ast_node * node) {
    struct vector * stmts;

    stmts = vector_get (node->children, 0);

    for (int i = 0; i < stmts->length; i++) {
        free_ast_node (vector_get (stmts, i));
    }

    free_vector (stmts);
}

static void free_class_node (struct ast_node * node) {
    struct vector * extends;

    free (vector_get (node->children, 0));

    extends = vector_get (node->children, 1);
    if (extends) {
        free_string_vector (extends);
    }

    free_ast_node (vector_get (node->children, 2));
}

static void free_closure_node (struct ast_node * node) {
    struct vector * params;
    struct vector * return_type;

    params      = vector_get (node->children, 0);
    for (int i = 0; i < params->length; i++) {
        free_func_param (vector_get (params, i));
    }
    free_vector (params);

    return_type = vector_get (node->children, 1);
    if (return_type) {
        free_string_vector (return_type);
    }

    free_ast_node (vector_get (node->children, 2));
}

static void free_expr_stmt_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
}

static void free_for_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
    free_ast_node (vector_get (node->children, 2));
    free_ast_node (vector_get (node->children, 3));
}

static void free_foreach_node (struct ast_node * node) {
    free          (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
    free_ast_node (vector_get (node->children, 2));
}

static void free_func_call_node (struct ast_node * node) {
    struct vector * args;

    free_ast_node (vector_get (node->children, 0));

    args = vector_get (node->children, 1);
    for (int i = 0; i < args->length; i++) {
        free_ast_node (vector_get (args, i));
    }
    free_vector (args);
}

static void free_func_decl_node (struct ast_node * node) {
    struct vector * params;
    struct vector * return_type;

    free (vector_get (node->children, 0));

    params      = vector_get (node->children, 1);
    for (int i = 0; i < params->length; i++) {
        free_func_param (vector_get (params, i));
    }
    free_vector (params);

    return_type = vector_get (node->children, 2);
    if (return_type) {
        free_string_vector (return_type);
    }

    free_ast_node (vector_get (node->children, 3));
}

static void free_func_param (struct func_param * param) {
    if (param->ids) {
        free_string_vector (param->ids);
    }

    if (param->id) {
        free (param->id);
    }

    if (param->enforced_type) {
        free_string_vector (param->enforced_type);
    }

    free (param);
}

static void free_id_node (struct ast_node * node) {
    free (vector_get (node->children, 0));
}

static void free_if_node (struct ast_node * node) {
    struct ast_node * else_body;

    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));

    else_body = vector_get (node->children, 2);
    if (else_body) {
        free_ast_node (else_body);
    }
}

static void free_import_node (struct ast_node * node) {
    char * str;

    str = vector_get (node->children, 1);

    if (str) {
        free (str);
    } else {
        free_string_vector (vector_get (node->children, 0));
    }
}

static void free_list_decl_node (struct ast_node * node) {
    struct vector * elements;

    elements = vector_get (node->children, 0);
    for (int i = 0; i < elements->length; i++) {
        free_ast_node (vector_get (elements, i));
    }
    free_vector (elements);
}

static void free_obj_decl_node (struct ast_node * node) {
    struct vector * vals;

    free_string_vector (vector_get (node->children, 0));

    vals = vector_get (node->children, 1);
    for (int i = 0; i < vals->length; i++) {
        free_ast_node (vector_get (vals, i));
    }
    free_vector (vals);
}

static void free_raise_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
}

static void free_return_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
}

static void free_string_node (struct ast_node * node) {
    free (vector_get (node->children, 0));
}

static void free_subscript_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
}

static void free_super_node (struct ast_node * node) {
    struct vector * args;

    args = vector_get (node->children, 0);
    for (int i = 0; i < args->length; i++) {
        free_ast_node (vector_get (args, i));
    }
    free_vector (args);
}

static void free_try_catch_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
    free          (vector_get (node->children, 2));
}

static void free_typeof_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
}

static void free_unary_op_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
}

static void free_use_node (struct ast_node * node) {
    free_string_vector (vector_get (node->children, 0));
    free_ast_node      (vector_get (node->children, 1));
}

static void free_while_node (struct ast_node * node) {
    free_ast_node (vector_get (node->children, 0));
    free_ast_node (vector_get (node->children, 1));
}
