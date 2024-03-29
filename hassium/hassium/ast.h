#ifndef _AST_H_
#define _AST_H_

#include <ds/vec.h>
#include <sourcefile.h>
#include <stdlib.h>

typedef enum {
  ARRAY_DECL_NODE,
  ATTRIB_NODE,
  BIN_OP_NODE,
  BREAK_NODE,
  CLASS_DECL_NODE,
  CONTINUE_NODE,
  CODE_BLOCK_NODE,
  DELETE_NODE,
  DO_WHILE_NODE,
  EMPTY_STMT,
  EXPR_STMT_NODE,
  FOR_NODE,
  FOREACH_NODE,
  FUNC_DECL_NODE,
  ID_NODE,
  IF_NODE,
  IMPORT_NODE,
  INVOKE_NODE,
  NUM_NODE,
  OBJ_DECL_NODE,
  RAISE_NODE,
  RETURN_NODE,
  SLICE_NODE,
  STRING_NODE,
  SUBSCRIPT_NODE,
  SUPER_NODE,
  SWITCH_NODE,
  TERNARY_NODE,
  TRY_CATCH_NODE,
  UNARY_OP_NODE,
  WHILE_NODE,
} ast_type_t;

static char *ast_type_t_names[] = {
    "array",
    "attribute access",
    "binary operation",
    "break",
    "class",
    "continue",
    "code block",
    "delete",
    "do while",
    ";",
    "expression statement",
    "for",
    "foreach",
    "func",
    "identifier",
    "if",
    "import",
    "invoke",
    "number",
    "object",
    "raise",
    "return",
    "slice",
    "string",
    "array indexer",
    "super",
    "switch",
    "ternary operator",
    "try catch",
    "unary operation",
    "while",
};

typedef enum {
  BIN_OP_ADD,
  BIN_OP_AND,
  BIN_OP_ASSIGN,
  BIN_OP_ASSIGN_SHORT,
  BIN_OP_DIV,
  BIN_OP_EQ,
  BIN_OP_GREATER,
  BIN_OP_GREATER_OR_EQ,
  BIN_OP_LESSER,
  BIN_OP_LESSER_OR_EQ,
  BIN_OP_MOD,
  BIN_OP_MUL,
  BIN_OP_OR,
  BIN_OP_SUB,
} bin_op_type_t;

typedef enum {
  UNARY_OP_NOT,
  UNARY_OP_POST_DEC,
  UNARY_OP_POST_INC,
  UNARY_OP_PRE_DEC,
  UNARY_OP_PRE_INC,
} unary_op_type_t;

struct ast_node {
  ast_type_t type;
  void *inner;
  struct sourcepos *sourcepos;
};

struct array_decl_node {
  struct vec *values;
};
struct attrib_node {
  struct ast_node *target;
  char *attrib;
  bool null_coalescing;
};

struct bin_op_node {
  bin_op_type_t type;
  struct ast_node *left;
  struct ast_node *right;
  bool for_switch;
};

struct class_decl_node {
  char *name;
  struct ast_node *extends;
  struct vec *impls;
  struct ast_node *body;
};

struct code_block_node {
  struct vec *children;
};

struct delete_node {
  struct ast_node *target;
};

struct expr_stmt_node {
  struct ast_node *expr;
};

struct for_node {
  struct ast_node *initial;
  struct ast_node *condition;
  struct ast_node *repeated;
  struct ast_node *body;
};

struct foreach_node {
  char *id;
  struct ast_node *target;
  struct ast_node *body;
};

struct func_decl_node {
  char *name;
  struct vec *params;
  struct ast_node *ret_type;
  struct ast_node *body;
};

struct id_node {
  char *id;
  struct ast_node *type;
};

struct if_node {
  struct ast_node *predicate;
  struct ast_node *body;
  struct ast_node *else_body;
};

struct import_node {
  struct vec *imports;
  struct vec *from;
};

struct invoke_node {
  struct ast_node *target;
  struct vec *args;
  bool null_coalescing;
};

struct num_node {
  bool is_float;
  int val_int;
  float val_float;
};

struct obj_decl_node {
  struct vec *keys;
  struct vec *values;
};

struct raise_node {
  struct ast_node *value;
};

struct return_node {
  struct ast_node *value;
};

struct slice_node {
  struct ast_node *target;
  struct ast_node *start;
  struct ast_node *end;
  bool null_coalescing;
};

struct string_node {
  char *value;
};

struct subscript_node {
  struct ast_node *target;
  struct ast_node *key;
  bool null_coalescing;
};

struct super_node {
  struct vec *args;
};

struct switch_node {
  struct ast_node *target;
  struct vec *cases;
  struct vec *case_bodies;
  struct ast_node *default_case;
};

struct ternary_node {
  struct ast_node *condition;
  struct ast_node *true_value;
  struct ast_node *false_value;
};

struct try_catch_node {
  char *id;
  struct ast_node *try;
  struct ast_node *catch;
};

struct unary_op_node {
  unary_op_type_t type;
  struct ast_node *target;
  bool for_switch;
};

struct while_node {
  struct ast_node *condition;
  struct ast_node *body;
  struct ast_node *else_body;
};

struct ast_node *ast_node_new(ast_type_t, void *, struct sourcepos *);
struct ast_node *array_decl_node_new(struct vec *, struct sourcepos *);
struct ast_node *attrib_node_new(struct ast_node *, char *, struct sourcepos *,
                                 bool);
struct ast_node *bin_op_node_new(bin_op_type_t, struct ast_node *,
                                 struct ast_node *, bool for_switch,
                                 struct sourcepos *);
struct ast_node *class_decl_node_new(char *, struct ast_node *,
                                     struct ast_node *, struct vec *,
                                     struct sourcepos *);
struct ast_node *code_block_node_new(struct vec *, struct sourcepos *);
struct ast_node *delete_node_new(struct ast_node *, struct sourcepos *);
struct ast_node *do_while_node_new(struct ast_node *, struct ast_node *,
                                   struct sourcepos *);
struct ast_node *expr_stmt_node_new(struct ast_node *, struct sourcepos *);
struct ast_node *for_node_new(struct ast_node *, struct ast_node *,
                              struct ast_node *, struct ast_node *,
                              struct sourcepos *);
struct ast_node *foreach_node_new(char *, struct ast_node *, struct ast_node *,
                                  struct sourcepos *);
struct ast_node *func_decl_node_new(char *, struct vec *, struct ast_node *,
                                    struct ast_node *, struct sourcepos *);
struct ast_node *id_node_new(char *, struct ast_node *, struct sourcepos *);
struct ast_node *if_node_new(struct ast_node *, struct ast_node *,
                             struct ast_node *, struct sourcepos *);
struct ast_node *import_node_new(struct vec *, struct vec *,
                                 struct sourcepos *);
struct ast_node *invoke_node_new(struct ast_node *, struct vec *,
                                 struct sourcepos *, bool);
struct ast_node *num_node_new(bool, int, float, struct sourcepos *);
struct ast_node *obj_decl_node_new(struct vec *, struct vec *,
                                   struct sourcepos *);
struct ast_node *raise_node_new(struct ast_node *, struct sourcepos *);
struct ast_node *return_node_new(struct ast_node *, struct sourcepos *);
struct ast_node *slice_node_new(struct ast_node *, struct ast_node *,
                                struct ast_node *, struct sourcepos *, bool);
struct ast_node *string_node_new(char *, struct sourcepos *);
struct ast_node *switch_node_new(struct ast_node *, struct vec *, struct vec *,
                                 struct ast_node *, struct sourcepos *);
struct ast_node *subscript_node_new(struct ast_node *, struct ast_node *,
                                    struct sourcepos *, bool);
struct ast_node *super_node_new(struct vec *, struct sourcepos *);
struct ast_node *ternary_node_new(struct ast_node *, struct ast_node *,
                                  struct ast_node *, struct sourcepos *);
struct ast_node *try_catch_node_new(struct ast_node *, struct ast_node *,
                                    char *, struct sourcepos *);
struct ast_node *unary_op_node_new(unary_op_type_t, struct ast_node *,
                                   bool for_switch, struct sourcepos *);
struct ast_node *while_node_new(struct ast_node *, struct ast_node *,
                                struct ast_node *, struct sourcepos *);

void ast_node_free(struct ast_node *);
void debug_ast(struct ast_node *);

#endif