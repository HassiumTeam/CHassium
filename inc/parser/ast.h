#ifndef _AST_H_
#define _AST_H_

#include <bin_op_type.h>
#include <parser/node_type.h>
#include <unary_op_type.h>
#include <vector.h>

struct ast_node {
    void        * state;
    node_type_t   type;
};

struct assign_state {
    struct ast_node * left;
    struct ast_node * right;
};

struct attrib_access_state {
    struct ast_node * target;
    char            * id;
};

struct bin_op_state {
    bin_op_type_t     bin_op_type;
    struct ast_node * left;
    struct ast_node * right;
};

struct block_state {
    struct vector_state * stmts;
};

struct class_state {
    char                * name;
    struct vector_state * extends;
    struct ast_node     * body;
};

struct closure_state {
    struct vector_state * params;
    struct vector_state * return_type;
    struct ast_node     * body;
};

struct expr_stmt_state {
    struct ast_node * expr;
};

struct for_state {
    struct ast_node * pre_stmt;
    struct ast_node * expr;
    struct ast_node * rep_stmt;
    struct ast_node * body;
};

struct foreach_state {
    char            * id;
    struct ast_node * target;
    struct ast_node * body;
};

struct func_call_state {
    struct ast_node     * target;
    struct vector_state * args;
};

struct func_decl_state {
    char                * id;
    struct vector_state * params;
    struct vector_state * return_type;
    struct ast_node     * body;
};

struct id_state {
    char * id;
};

struct if_state {
    struct ast_node * if_expr;
    struct ast_node * if_body;
    struct ast_node * else_body;
};

struct import_state {
    char * mod;
};

struct list_decl_state {
    struct vector_state * elements;
};

struct number_state {
    float f;
};

struct obj_decl_state {
    struct vector_state * ids;
    struct vector_state * vals;
};

struct raise_state {
    struct ast_node * expr;
};

struct return_state {
    struct ast_node * expr;
};

struct string_state {
    char * str;
};

struct subscript_state {
    struct ast_node * target;
    struct ast_node * index;
};

struct try_catch_state {
    struct ast_node * try_body;
    struct ast_node * catch_body;
};

struct typeof_state {
    struct ast_node * expr;
};

struct unary_op_state {
    unary_op_type_t   type;
    struct ast_node * target;
};

struct use_state {
    struct vector_state * imports;
    char                * mod;
};

struct while_state {
    struct ast_node * expr;
    struct ast_node * body;
};

#endif
