#ifndef _AST_H_
#define _AST_H_

#include <bin_op_type.h>
#include <parser/node_type.h>
#include <stdlib.h>
#include <stdio.h>
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
    bin_op_type_t     type;
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
    char                * name;
    struct vector_state * params;
    struct vector_state * return_type;
    struct ast_node     * body;
};

struct id_state {
    char * id;
};

struct if_state {
    struct ast_node * expr;
    struct ast_node * if_body;
    struct ast_node * else_body;
};

struct import_state {
    char                * file;
    struct vector_state * name;
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

struct super_state {
    struct vector_state * args;
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

struct ast_node * assign_node_init (struct ast_node * left,
                                    struct ast_node * right);
void assign_node_free (struct ast_node * node);
struct ast_node * attrib_access_node_init (struct ast_node * target,
                                           char            * id);
void attrib_access_node_free (struct ast_node * node);
struct ast_node * bin_op_node_init (bin_op_type_t     bin_op_type,
                                    struct ast_node * left,
                                    struct ast_node * right);
void bin_op_node_free (struct ast_node * node);
struct ast_node * block_node_init (struct vector_state * stmts);
void block_node_free (struct ast_node * node);
struct ast_node * class_node_init (char                * name,
                                   struct vector_state * extends,
                                   struct ast_node     * body);
void class_node_free (struct ast_node * node);
struct ast_node * break_node_init ();
void break_node_free (struct ast_node * node);
struct ast_node * closure_node_init (struct vector_state * params,
                                     struct vector_state * return_type,
                                     struct ast_node     * body);
void closure_node_free (struct ast_node * node);
struct ast_node * continue_node_init ();
void continue_node_free (struct ast_node * node);
struct ast_node * expr_stmt_node_init (struct ast_node * expr);
void expr_stmt_node_free (struct ast_node * node);
struct ast_node * for_node_init (struct ast_node * pre_stmt,
                                 struct ast_node * expr,
                                 struct ast_node * rep_stmt,
                                 struct ast_node * body);
void for_node_free (struct ast_node * node);
struct ast_node * foreach_node_init (char            * id,
                                     struct ast_node * target,
                                     struct ast_node * body);
void foreach_node_free (struct ast_node * node);
struct ast_node * func_call_node_init (struct ast_node     * target,
                                       struct vector_state * args);
void func_call_node_free (struct ast_node * node);
struct ast_node * func_decl_node_init (char                * name,
                                       struct vector_state * params,
                                       struct vector_state * return_type,
                                       struct ast_node     * body);
void func_decl_node_free (struct ast_node * node);
struct ast_node * id_node_init (char * id);
void id_node_free (struct ast_node * node);
struct ast_node * if_node_init (struct ast_node * expr,
                                struct ast_node * if_body,
                                struct ast_node * else_body);
void if_node_free (struct ast_node * node);
struct ast_node * import_node_init (char * file, struct vector_state * name);
void import_node_free (struct ast_node * node);
struct ast_node * list_decl_node_init (struct vector_state * elements);
void list_decl_node_free (struct ast_node * node);
struct ast_node * number_node_init (float f);
void number_node_free (struct ast_node * node);
struct ast_node * obj_decl_node_init (struct vector_state * ids,
                                      struct vector_state * vals);
void obj_decl_node_free (struct ast_node * node);
struct ast_node * raise_node_init (struct ast_node * expr);
void raise_node_free (struct ast_node * node);
struct ast_node * return_node_init (struct ast_node * expr);
void return_node_free (struct ast_node * node);
struct ast_node * string_node_init (char * str);
void string_node_free (struct ast_node * node);
struct ast_node * subscript_node_init (struct ast_node * target,
                                       struct ast_node * index);
void subscript_node_free (struct ast_node * node);
struct ast_node * super_node_init (struct vector_state * args);
void super_node_free (struct ast_node * node);
struct ast_node * try_catch_node_init (struct ast_node * try_body,
                                       struct ast_node * catch_body);
void try_catch_node_free (struct ast_node * node);
struct ast_node * typeof_node_init (struct ast_node * expr);
void typeof_node_free (struct ast_node * node);
struct ast_node * unary_op_node_init (unary_op_type_t   type,
                                      struct ast_node * target);
void unary_op_node_free (struct ast_node * node);
struct ast_node * use_node_init (struct vector_state * import,
                                 char                * mod);
void use_node_free (struct ast_node * node);
struct ast_node * while_node_init (struct ast_node * expr,
                                   struct ast_node * body);
void while_node_free (struct ast_node * node);

#endif
