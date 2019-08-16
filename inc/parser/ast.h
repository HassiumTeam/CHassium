#ifndef _AST_H_
#define _AST_H_

#include <bin_op_type.h>
#include <parser/node_type.h>

struct ast_node {
    void * state;
    node_type_t type;
};

struct assign_node_state {
    struct ast_node * left;
    struct ast_node * right;
};

struct attrib_access_node_state {
    struct ast_node * target;
    char * id;
};

struct bin_op_node_state {
    bin_op_type_t bin_op_type;
    struct ast_node * left;
    struct ast_node * right;
};

#endif
