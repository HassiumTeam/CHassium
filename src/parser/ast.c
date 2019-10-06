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

void free_ast_node (struct ast_node * node) {
    free_vector (node->children);
    free        (node);
}
