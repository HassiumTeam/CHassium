#include <parser/ast.h>

static struct ast_node * ast_node_init (node_type_t type) {
    struct ast_node * node = (struct ast_node *)malloc (sizeof (struct ast_node));
    node->type = type;

    return node;
}

struct ast_node * assign_node_init (struct ast_node * left, struct ast_node * right) {
    struct assign_state * state;
    struct ast_node     * node;

    state = (struct assign_state *)malloc (sizeof (struct assign_state));
    state->left  = left;
    state->right = right;

    node = ast_node_init (assign_node);
    node->state = state;

    return node;
}

struct ast_node * attrib_access_node_init (struct ast_node * target, char * id) {
    struct attrib_access_state * state;
    struct ast_node            * node;

    state = (struct attrib_access_state *)malloc (sizeof (struct attrib_access_state));
    state->target = target;
    state->id     = id;

    node = ast_node_init (attrib_access_node);
    node->state = state;

    return node;
}

struct ast_node * bin_op_node_init (bin_op_type_t type, struct ast_node * left, struct ast_node * right) {
    struct bin_op_state * state;
    struct ast_node     * node;

    state = (struct bin_op_state *)malloc (sizeof (struct bin_op_state));
    state->type  = type;
    state->left  = left;
    state->right = right;

    node = ast_node_init (bin_op_node);
    node->state = state;

    return state;
}
