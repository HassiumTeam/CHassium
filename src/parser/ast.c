#include <parser/ast.h>

static struct ast_node * ast_node_init (node_type_t type) {
    struct ast_node * node = (struct ast_node *)calloc (1, sizeof (struct ast_node));
    node->type = type;

    return node;
}

struct ast_node * assign_node_init (struct ast_node * left, struct ast_node * right) {
    struct assign_state * state;
    struct ast_node     * node;

    state = (struct assign_state *)calloc (1, sizeof (struct assign_state));
    state->left  = left;
    state->right = right;

    node = ast_node_init (assign_node);
    node->state = state;

    return node;
}

struct ast_node * attrib_access_node_init (struct ast_node * target, char * id) {
    struct attrib_access_state * state;
    struct ast_node            * node;

    state = (struct attrib_access_state *)calloc (1, sizeof (struct attrib_access_state));
    state->target = target;
    state->id     = id;

    node = ast_node_init (attrib_access_node);
    node->state = state;

    return node;
}

struct ast_node * bin_op_node_init (bin_op_type_t type, struct ast_node * left, struct ast_node * right) {
    struct bin_op_state * state;
    struct ast_node     * node;

    state = (struct bin_op_state *)calloc (1, sizeof (struct bin_op_state));
    state->type  = type;
    state->left  = left;
    state->right = right;

    node = ast_node_init (bin_op_node);
    node->state = state;

    return node;
}

struct ast_node * block_node_init (struct vector_state * stmts) {
    struct block_state * state;
    struct ast_node    * node;

    state = (struct block_state *)calloc (1, sizeof (struct block_state));
    state->stmts = stmts;

    node = ast_node_init (block_node);
    node->state = state;

    return node;
}

struct ast_node * break_node_init () {
    struct ast_node * node;

    node = ast_node_init (break_node);
    node->state = 0;

    return node;
}

struct ast_node * class_node_init (char * name, struct vector_state * extends, struct ast_node * body) {
    struct class_state * state;
    struct ast_node    * node;

    state = (struct class_state *)calloc (1, sizeof (struct class_state));
    state->name    = name;
    state->extends = extends;
    state->body    = body;

    node = ast_node_init (class_node);
    node->state = state;

    return node;
}

struct ast_node * closure_node_init (struct vector_state * params, struct vector_state * return_type, struct ast_node * body) {
    struct closure_state * state;
    struct ast_node      * node;

    state = (struct closure_state *)calloc (1, sizeof (struct closure_state));
    state->params      = params;
    state->return_type = return_type;
    state->body        = body;

    node = ast_node_init (closure_node);
    node->state = state;

    return node;
}

struct ast_node * continue_node_init () {
    struct ast_node * node;

    node = ast_node_init (continue_node);
    node->state = 0;

    return node;
}

struct ast_node * expr_stmt_node_init (struct ast_node * expr) {
    struct expr_stmt_state * state;
    struct ast_node        * node;

    state = (struct expr_stmt_state *)calloc (1, sizeof (struct expr_stmt_state));
    state->expr = expr;

    node = ast_node_init (expr_stmt_node);
    node->state = state;

    return node;
}

struct ast_node * for_node_init (struct ast_node * pre_stmt, struct ast_node * expr, struct ast_node * rep_stmt, struct ast_node * body) {
    struct for_state * state;
    struct ast_node *  node;

    state = (struct for_state *)calloc (1, sizeof (struct for_state));
    state->pre_stmt = pre_stmt;
    state->expr     = expr;
    state->rep_stmt = rep_stmt;
    state->body     = body;

    node = ast_node_init (for_node);
    node->state = state;

    return node;
}

struct ast_node * foreach_node_init (char * id, struct ast_node * target, struct ast_node * body) {
    struct foreach_state * state;
    struct ast_node      * node;

    state = (struct foreach_state *)calloc (1, sizeof (struct foreach_state));
    state->id     = id;
    state->target = target;
    state->body   = body;

    node = ast_node_init (foreach_node);
    node->state = state;

    return node;
}

struct ast_node * func_call_node_init (struct ast_node * target, struct vector_state * args) {
    struct func_call_state * state;
    struct ast_node        * node;

    state = (struct func_call_state *)calloc (1, sizeof (struct func_call_state));
    state->target = target;
    state->args   = args;

    node = ast_node_init (func_call_node);
    node->state = state;

    return node;
}

struct ast_node * func_decl_node_init (char * id, struct vector_state * params, struct vector_state * return_type, struct ast_node * body) {
    struct func_decl_state * state;
    struct ast_node        * node;

    state = (struct func_decl_state *)calloc (1, sizeof (struct func_decl_state));
    state->id          = id;
    state->params      = params;
    state->return_type = return_type;
    state->body        = body;

    node = ast_node_init (func_decl_node);
    node->state = state;

    return node;
}

struct ast_node * id_node_init (char * id) {
    struct id_state * state;
    struct ast_node * node;

    state = (struct id_state *)calloc (1, sizeof (struct id_state));
    state->id = id;

    node = ast_node_init (id_node);
    node->state = state;

    return node;
}

struct ast_node * if_node_init (struct ast_node * expr, struct ast_node * if_body, struct ast_node * else_body) {
    struct if_state * state;
    struct ast_node * node;

    state = (struct if_state *)calloc (1, sizeof (struct if_state));
    state->expr      = expr;
    state->if_body   = if_body;
    state->else_body = else_body;

    node = ast_node_init (if_node);
    node->state = state;

    return node;
}

struct ast_node * import_init (char * mod) {
    struct import_state * state;
    struct ast_node     * node;

    state = (struct import_state *)calloc (1, sizeof (struct import_state));
    state->mod = mod;

    node = ast_node_init (import_node);
    node->state = state;

    return node;
}

struct ast_node * list_decl_node_init (struct vector_state * elements) {
    struct list_decl_state * state;
    struct ast_node        * node;

    state = (struct list_decl_state *)calloc (1, sizeof (struct list_decl_state));
    state->elements = elements;

    node = ast_node_init (list_decl_node);
    node->state = state;

    return node;
}

struct ast_node * number_node_init (float f) {
    struct number_state * state;
    struct ast_node     * node;

    state = (struct number_state *)calloc (1, sizeof (struct number_state));
    state->f = f;

    node = ast_node_init (number_node);
    node->state = state;

    return node;
}

struct ast_node * obj_decl_node_init (struct vector_state * ids, struct vector_state * vals) {
    struct obj_decl_state * state;
    struct ast_node       * node;

    state = (struct obj_decl_state *)calloc (1, sizeof (struct obj_decl_state));
    state->ids  = ids;
    state->vals = vals;

    node = ast_node_init (obj_decl_node);
    node->state = state;

    return node;
}

struct ast_node * raise_node_init (struct ast_node * expr) {
    struct raise_state * state;
    struct ast_node    * node;

    state = (struct raise_state *)calloc (1, sizeof (struct raise_state));
    state->expr = expr;

    node = ast_node_init (raise_node);
    node->state = state;

    return node;
}

struct ast_node * return_node_init (struct ast_node * expr) {
    struct return_state * state;
    struct ast_node     * node;

    state = (struct return_state *)calloc (1, sizeof (struct return_state));
    state->expr = expr;

    node = ast_node_init (return_node);
    node->state = state;

    return node;
}

struct ast_node * string_node_init (char * str) {
    struct string_state * state;
    struct ast_node     * node;

    state = (struct string_state *)calloc (1, sizeof (struct string_state));
    state->str = str;

    node = ast_node_init (string_node);
    node->state = state;

    return node;
}

struct ast_node * subscript_node_init (struct ast_node * target, struct ast_node * index) {
    struct subscript_state * state;
    struct ast_node        * node;

    state = (struct subscript_state *)calloc (1, sizeof (struct subscript_state));
    state->target = target;
    state->index  = index;

    node = ast_node_init (subscript_node);
    node->state = state;

    return node;
}

struct ast_node * try_catch_node_init (struct ast_node * try_body, struct ast_node * catch_body) {
    struct try_catch_state * state;
    struct ast_node        * node;

    state = (struct try_catch_state *)calloc (1, sizeof (struct try_catch_state));
    state->try_body = try_body;
    state->catch_body = catch_body;

    node = ast_node_init (try_catch_node);
    node->state = state;

    return node;
}

struct ast_node * typeof_node_init (struct ast_node * expr) {
    struct typeof_state * state;
    struct ast_node     * node;

    state = (struct typeof_state *)calloc (1, sizeof (struct typeof_state));
    state->expr = expr;

    node = ast_node_init (typeof_node);
    node->state = state;

    return node;
}

struct ast_node * unary_op_node_init (unary_op_type_t type, struct ast_node * target) {
    struct unary_op_state * state;
    struct ast_node       * node;

    state = (struct unary_op_state *)calloc (1, sizeof (struct unary_op_state));
    state->type = type;
    state->target = target;

    node = ast_node_init (unary_op_node);
    node->state = state;

    return node;
}

struct ast_node * use_node_init (struct vector_state * imports, char * mod) {
    struct use_state * state;
    struct ast_node  * node;

    state = (struct use_state *)calloc (1, sizeof (struct use_state));
    state->imports = imports;
    state->mod     = mod;

    node = ast_node_init (use_node);
    node->state = state;

    return node;
}

struct ast_node * while_node_init (struct ast_node * expr, struct ast_node * body) {
    struct while_state * state;
    struct ast_node    * node;

    state = (struct while_state *)calloc (1, sizeof (struct while_state));
    state->expr = expr;
    state->body = body;

    node = ast_node_init (while_node);
    node->state = state;

    return node;
}
