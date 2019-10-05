#ifndef _AST_H_
#define _AST_H_

#include <stdarg.h>
#include <stdlib.h>
#include <util/vector.h>

typedef enum {
    ASSIGN_NODE,  ATTRIB_ACCESS_NODE, BIN_OP_NODE,      BLOCK_NODE,     BREAK_NODE,
    CLASS_NODE,   CLOSURE_NODE,       CONTINUE_NODE,    EXPR_STMT_NODE, FOR_NODE,
    FOREACH_NODE, FUNC_CALL_NODE,     FUNC_DECL_NODE,   ID_NODE,        IF_NODE,
    IMPORT_NODE,  LIST_DECL_NODE,     NUMBER_NODE,      OBJ_DECL_NODE,  RAISE_NODE,
    RETURN_NODE,  STRING_NODE,        SUBSCRIPT_NODE,   SUPER_NODE,     TRY_CATCH_NODE,
    TYPEOF_NODE,  UNARY_OP_NODE,      USE_NODE,         WHILE_NODE,
} node_type_t;

typedef enum {
    ADD_BIN_OP,    BITWISE_AND_BIN_OP,     BITWISE_OR_BIN_OP,       DIV_BIN_OP,
    EQUAL_BIN_OP,  GREATER_BIN_OP,         GREATER_OR_EQUAL_BIN_OP, INSTANCEOF_BIN_OP,
    LESSER_BIN_OP, LESSER_OR_EQUAL_BIN_OP, LOGICAL_AND_BIN_OP,      LOGICAL_OR_BIN_OP,
    MOD_BIN_OP,    MUL_BIN_OP, SUB_BIN_OP, XOR_BIN_OP,
} bin_op_type_t;

typedef enum {
    LOGICAL_NOT_UNARY_OP, POST_DEC_UNARY_OP, POST_INC_UNARY_OP, PRE_DEC_UNARY_OP,
    PRE_INC_UNARY_OP,
} unary_op_type_t;

struct ast_node {
    node_type_t     type;
    struct vector * children;
    int             flag1;
    int             flag2;
};

struct ast_node * ast_node_init (node_type_t type, int flag1, int flag2, int count, ...);
void              free_ast_node (struct ast_node * node);

#endif
