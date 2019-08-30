#include <parser/ast.h>

void ast_node_free (struct ast_node * ast) {
    switch (ast->type) {
        case assign_node:
            assign_node_free (ast);
            break;
        case attrib_access_node:
            attrib_access_node_free (ast);
            break;
        case bin_op_node:
            bin_op_node_free (ast);
            break;
        case block_node:
            block_node_free (ast);
            break;
        case break_node:
            break;
        case class_node:
            class_node_free (ast);
            break;
        case closure_node:
            closure_node_free (ast);
            break;
        case continue_node:
            break;
        case expr_stmt_node:
            expr_stmt_node_free (ast);
            break;
        case for_node:
            for_node_free (ast);
            break;
        case foreach_node:
            foreach_node_free (ast);
            break;
        case func_call_node:
            func_call_node_free (ast);
            break;
        case func_decl_node:
            func_decl_node_free (ast);
            break;
        case id_node:
            id_node_free (ast);
            break;
        case if_node:
            if_node_free (ast);
            break;
        case import_node:
            import_node_free (ast);
            break;
        case list_decl_node:
            list_decl_node_free (ast);
            break;
        case number_node:
            number_node_free (ast);
            break;
        case obj_decl_node:
            obj_decl_node_free (ast);
            break;
        case raise_node:
            raise_node_free (ast);
            break;
        case return_node:
            return_node_free (ast);
            break;
        case string_node:
            string_node_free (ast);
            break;
        case subscript_node:
            subscript_node_free (ast);
            break;
        case super_node:
            super_node_free (ast);
            break;
        case try_catch_node:
            try_catch_node_free (ast);
            break;
        case typeof_node:
            typeof_node_free (ast);
            break;
        case unary_op_node:
            unary_op_node_free (ast);
            break;
        case use_node:
            use_node_free (ast);
            break;
        case while_node:
            while_node_free (ast);
            break;
    }
    if (ast->state) {
        free (ast->state);
    }
    free (ast);
};

static struct ast_node * ast_node_init (node_type_t type) {
    struct ast_node * node = (struct ast_node *)calloc (1, sizeof (struct ast_node));
    node->type = type;

    return node;
}

struct ast_node * assign_node_init (struct ast_node * left, struct ast_node * right, int free_left) {
    struct assign_state * state;
    struct ast_node     * node;

    state = (struct assign_state *)calloc (1, sizeof (struct assign_state));
    state->left      = left;
    state->right     = right;
    state->free_left = free_left;

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

    state        = (struct block_state *)calloc (1, sizeof (struct block_state));
    state->stmts = stmts;

    node        = ast_node_init (block_node);
    node->state = state;

    return node;
}

struct ast_node * break_node_init () {
    struct ast_node * node;

    node        = ast_node_init (break_node);
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

struct ast_node * func_decl_node_init (char * name, struct vector_state * params, struct vector_state * return_type, struct ast_node * body) {
    struct func_decl_state * state;
    struct ast_node        * node;

    state = (struct func_decl_state *)calloc (1, sizeof (struct func_decl_state));
    state->name        = name;
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

struct ast_node * import_node_init (char * file, struct vector_state * name) {
    struct import_state * state;
    struct ast_node     * node;

    state = (struct import_state *)calloc (1, sizeof (struct import_state));
    state->file = file;
    state->name = name;

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

    state         = (struct subscript_state *)calloc (1, sizeof (struct subscript_state));
    state->target = target;
    state->index  = index;

    node        = ast_node_init (subscript_node);
    node->state = state;

    return node;
}

struct ast_node * super_node_init (struct vector_state * args) {
    struct super_state * state;
    struct ast_node    * node;

    state       = (struct super_state *)malloc (sizeof (struct super_state));
    state->args = args;

    node        = ast_node_init (super_node);
    node->state = state;

    return node;
}

struct ast_node * try_catch_node_init (struct ast_node * try_body, struct ast_node * catch_body) {
    struct try_catch_state * state;
    struct ast_node        * node;

    state = (struct try_catch_state *)calloc (1, sizeof (struct try_catch_state));
    state->try_body = try_body;
    state->catch_body = catch_body;

    node        = ast_node_init (try_catch_node);
    node->state = state;

    return node;
}

struct ast_node * typeof_node_init (struct ast_node * expr) {
    struct typeof_state * state;
    struct ast_node     * node;

    state = (struct typeof_state *)calloc (1, sizeof (struct typeof_state));
    state->expr = expr;

    node        = ast_node_init (typeof_node);
    node->state = state;

    return node;
}

struct ast_node * unary_op_node_init (unary_op_type_t type, struct ast_node * target) {
    struct unary_op_state * state;
    struct ast_node       * node;

    state = (struct unary_op_state *)calloc (1, sizeof (struct unary_op_state));
    state->type = type;
    state->target = target;

    node        = ast_node_init (unary_op_node);
    node->state = state;

    return node;
}

struct ast_node * use_node_init (struct vector_state * imports, struct ast_node * expr) {
    struct use_state * state;
    struct ast_node  * node;

    state          = (struct use_state *)calloc (1, sizeof (struct use_state));
    state->imports = imports;
    state->expr    = expr;

    node        = ast_node_init (use_node);
    node->state = state;

    return node;
}

struct ast_node * while_node_init (struct ast_node * expr, struct ast_node * body) {
    struct while_state * state;
    struct ast_node    * node;

    state       = (struct while_state *)calloc (1, sizeof (struct while_state));
    state->expr = expr;
    state->body = body;

    node        = ast_node_init (while_node);
    node->state = state;

    return node;
}

void assign_node_free (struct ast_node * node) {
    struct assign_state * state = (struct assign_state *)node->state;

    if (state->free_left) {
        ast_node_free (state->left);
    }
    ast_node_free (state->right);
}

void attrib_access_node_free (struct ast_node * node) {
    struct attrib_access_state * state = (struct attrib_access_state *)node->state;

    ast_node_free (state->target);
    free          (state->id);
}

void bin_op_node_free (struct ast_node * node) {
    struct bin_op_state * state = (struct bin_op_state *)node->state;

    ast_node_free (state->left);
    ast_node_free (state->right);
}

void block_node_free (struct ast_node * node) {
    struct block_state * state = (struct block_state *)node->state;

    for (int i = 0; i < state->stmts->length; i++) {
        ast_node_free (vector_get (state->stmts, i));
    }
    vector_free (state->stmts);
}

void class_node_free (struct ast_node * node) {
    struct class_state * state = (struct class_state *)node->state;

    ast_node_free (state->body);
}

void closure_node_free (struct ast_node * node) {
    struct closure_state * state = (struct closure_state *)node->state;

    ast_node_free (state->body);
}

void expr_stmt_node_free (struct ast_node * node) {
    struct expr_stmt_state * state = (struct expr_stmt_state *)node->state;

    ast_node_free (state->expr);
}

void for_node_free (struct ast_node * node) {
    struct for_state * state = (struct for_state *)node->state;

    ast_node_free (state->pre_stmt);
    ast_node_free (state->expr);
    ast_node_free (state->rep_stmt);
    ast_node_free (state->body);
}

void foreach_node_free (struct ast_node * node) {
    struct foreach_state * state = (struct foreach_state *)node->state;

    free          (state->id);
    ast_node_free (state->target);
    ast_node_free (state->body);
}

void func_call_node_free (struct ast_node * node) {
    struct func_call_state * state = (struct func_call_state *)node->state;

    ast_node_free (state->target);
    for (int i = 0; i < state->args->length; i++) {
        ast_node_free (vector_get (state->args, i));
    }
    vector_free   (state->args);
}

void func_decl_node_free (struct ast_node * node) {
    struct func_decl_state * state = (struct func_decl_state *)node->state;

    ast_node_free     (state->body);
}

void id_node_free (struct ast_node * node) {
    struct id_state * state = (struct id_state *)node->state;

    free (state->id);
}

void if_node_free (struct ast_node * node) {
    struct if_state * state = (struct if_state *)node->state;

    ast_node_free (state->expr);
    ast_node_free (state->if_body);
    if (state->else_body) {
        ast_node_free (state->else_body);
    }
}

void import_node_free (struct ast_node * node) {
    struct import_state * state = (struct import_state *)node->state;

    if (state->file) {
        free (state->file);
    }
    if (state->name) {
        access_chain_free (state->name);
    }
}

void list_decl_node_free (struct ast_node * node) {
    struct list_decl_state * state = (struct list_decl_state *)node->state;

    for (int i = 0; i < state->elements->length; i++) {
        ast_node_free (vector_get (state->elements, i));
    }
    vector_free (state->elements);
}

void number_node_free (struct ast_node * node) {

}

void obj_decl_node_free (struct ast_node * node) {
    struct obj_decl_state * state = (struct obj_decl_state *)node->state;

    for (int i = 0; i < state->vals->length; i++) {
        ast_node_free (vector_get (state->vals, i));
    }
    vector_free (state->vals);
}

void raise_node_free (struct ast_node * node) {
    struct raise_state * state = (struct raise_state *)node->state;

    ast_node_free (state->expr);
}

void return_node_free (struct ast_node * node) {
    struct return_state * state = (struct return_state *)node->state;

    ast_node_free (state->expr);
}

void string_node_free (struct ast_node * node) {
    struct string_state * state = (struct string_state *)node->state;

    free (state->str);
}

void subscript_node_free (struct ast_node * node) {
    struct subscript_state * state = (struct subscript_state *)node->state;

    ast_node_free (state->target);
    ast_node_free (state->index);
}

void super_node_free (struct ast_node * node) {
    struct super_state * state = (struct super_state *)node->state;

    for (int i = 0; i < state->args->length; i++) {
        ast_node_free (vector_get (state->args, i));
    }
    vector_free (state->args);
}

void try_catch_node_free (struct ast_node * node) {
    struct try_catch_state * state = (struct try_catch_state *)node->state;

    ast_node_free (state->try_body);
    ast_node_free (state->catch_body);
}

void typeof_node_free (struct ast_node * node) {
    struct typeof_state * state = (struct typeof_state *)node->state;

    ast_node_free (state->expr);
}

void unary_op_node_free (struct ast_node * node) {
    struct unary_op_state * state = (struct unary_op_state *)node->state;

    ast_node_free (state->target);
}

void use_node_free (struct ast_node * node) {
    struct use_state * state = (struct use_state *)node->state;

    for (int i = 0; i < state->imports->length; i++) {
        free (vector_get (state->imports, i));
    }
    vector_free   (state->imports);

    if (state->expr->type == string_node) {
        free (state->expr->state);
        free (state->expr);
    } else {
        ast_node_free (state->expr);
    }
}

void while_node_free (struct ast_node * node) {
    struct while_state * state = (struct while_state *)node->state;

    ast_node_free (state->expr);
    ast_node_free (state->body);
}

void access_chain_free (struct vector_state * chain) {
    for (int i = 0; i < chain->length; i++) {
        free (vector_get (chain, i));
    }
    vector_free (chain);
}

void param_list_free (struct vector_state * args) {
    struct func_param * param;

    for (int i = 0; i < args->length; i++) {
        param = vector_get (args, i);
        if (param->id) {
            free (param->id);
        }
        if (param->ids) {
            for (int j = 0; j < param->ids->length; j++) {
                free (vector_get (param->ids, j));
            }
            vector_free (param->ids);
        }
        if (param->symbols) {
            int_vector_free (param->symbols);
        }
        if (param->enforced_type) {
            access_chain_free (param->enforced_type);
        }
        free (param);
    }
    vector_free (args);
}
