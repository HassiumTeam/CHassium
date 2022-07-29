#include <ast.h>

struct ast_node *ast_node_new(ast_type_t type, void *inner) {
  struct ast_node *node = (struct ast_node *)calloc(1, sizeof(struct ast_node));
  node->type = type;
  node->inner = inner;
  return node;
}

struct ast_node *array_decl_node_new(struct vec *values) {
  struct array_decl_node *inner =
      (struct array_decl_node *)calloc(1, sizeof(struct array_decl_node));
  inner->values = values;
  return ast_node_new(ARRAY_DECL_NODE, inner);
}

struct ast_node *attrib_node_new(struct ast_node *target, char *attrib) {
  struct attrib_node *inner =
      (struct attrib_node *)calloc(1, sizeof(struct attrib_node));
  inner->target = target;
  inner->attrib = attrib;
  return ast_node_new(ATTRIB_NODE, inner);
}

struct ast_node *bin_op_node_new(bin_op_type_t type, struct ast_node *left,
                                 struct ast_node *right) {
  struct bin_op_node *inner =
      (struct bin_op_node *)calloc(1, sizeof(struct bin_op_node));
  inner->type = type;
  inner->left = left;
  inner->right = right;
  return ast_node_new(BIN_OP_NODE, inner);
}

struct ast_node *class_decl_node_new(char *name, struct ast_node *extends,
                                     struct ast_node *body) {
  struct class_decl_node *inner =
      (struct class_decl_node *)calloc(1, sizeof(struct class_decl_node));
  inner->name = name;
  inner->extends = extends;
  inner->body = body;
  return ast_node_new(CLASS_DECL_NODE, inner);
}

struct ast_node *code_block_node_new(struct vec *children) {
  struct code_block_node *inner =
      (struct code_block_node *)calloc(1, sizeof(struct code_block_node));
  inner->children = children;
  return ast_node_new(CODE_BLOCK_NODE, inner);
}

struct ast_node *expr_stmt_node_new(struct ast_node *expr) {
  struct expr_stmt_node *inner =
      (struct expr_stmt_node *)calloc(1, sizeof(struct expr_stmt_node));
  inner->expr = expr;
  return ast_node_new(EXPR_STMT_NODE, inner);
}

struct ast_node *for_node_new(struct ast_node *initial,
                              struct ast_node *condition,
                              struct ast_node *repeated,
                              struct ast_node *body) {
  struct for_node *inner =
      (struct for_node *)calloc(1, sizeof(struct for_node));
  inner->initial = initial;
  inner->condition = condition;
  inner->repeated = repeated;
  inner->body = body;
  return ast_node_new(FOR_NODE, inner);
}

struct ast_node *foreach_node_new(char *id, struct ast_node *target,
                                  struct ast_node *body) {
  struct foreach_node *inner =
      (struct foreach_node *)calloc(1, sizeof(struct foreach_node));
  inner->id = id;
  inner->target = target;
  inner->body = body;
  return ast_node_new(FOREACH_NODE, inner);
}

struct ast_node *func_decl_node_new(char *name, struct vec *params,
                                    struct ast_node *ret_type,
                                    struct ast_node *body) {
  struct func_decl_node *inner =
      (struct func_decl_node *)calloc(1, sizeof(struct func_decl_node));
  inner->name = name;
  inner->params = params;
  inner->ret_type = ret_type;
  inner->body = body;
  return ast_node_new(FUNC_DECL_NODE, inner);
}

struct ast_node *id_node_new(char *id) {
  struct id_node *inner = (struct id_node *)calloc(1, sizeof(struct id_node));
  inner->id = id;
  return ast_node_new(ID_NODE, inner);
}

struct ast_node *if_node_new(struct ast_node *predicate, struct ast_node *body,
                             struct ast_node *else_body) {
  struct if_node *inner = (struct if_node *)calloc(1, sizeof(struct if_node));
  inner->predicate = predicate;
  inner->body = body;
  inner->else_body = else_body;
  return ast_node_new(IF_NODE, inner);
}

struct ast_node *import_node_new(struct ast_node *target) {
  struct import_node *inner =
      (struct import_node *)calloc(1, sizeof(struct import_node));
  inner->target = target;
  return ast_node_new(IMPORT_NODE, inner);
}

struct ast_node *invoke_node_new(struct ast_node *target, struct vec *args) {
  struct invoke_node *inner =
      (struct invoke_node *)calloc(1, sizeof(struct invoke_node));
  inner->target = target;
  inner->args = args;
  return ast_node_new(INVOKE_NODE, inner);
}

struct ast_node *num_node_new(float value) {
  struct num_node *inner =
      (struct num_node *)calloc(1, sizeof(struct num_node));
  inner->value = value;
  return ast_node_new(NUM_NODE, inner);
}

struct ast_node *obj_decl_node_new(struct vec *keys, struct vec *values) {
  struct obj_decl_node *inner =
      (struct obj_decl_node *)calloc(1, sizeof(struct obj_decl_node));
  inner->keys = keys;
  inner->values = values;
  return ast_node_new(OBJ_DECL_NODE, inner);
}

struct ast_node *raise_node_new(struct ast_node *value) {
  struct raise_node *inner =
      (struct raise_node *)calloc(1, sizeof(struct raise_node));
  inner->value = value;
  return ast_node_new(RAISE_NODE, inner);
}

struct ast_node *return_node_new(struct ast_node *value) {
  struct return_node *inner =
      (struct return_node *)calloc(1, sizeof(struct return_node));
  inner->value = value;
  return ast_node_new(RETURN_NODE, inner);
}

struct ast_node *string_node_new(char *value) {
  struct string_node *inner =
      (struct string_node *)calloc(1, sizeof(struct string_node));
  inner->value = value;
  return ast_node_new(STRING_NODE, inner);
}

struct ast_node *subscript_node_new(struct ast_node *target,
                                    struct ast_node *key) {
  struct subscript_node *inner =
      (struct subscript_node *)calloc(1, sizeof(struct subscript_node));
  inner->target = target;
  inner->key = key;
  return ast_node_new(SUBSCRIPT_NODE, inner);
}

struct ast_node *super_node_new(struct vec *args) {
  struct super_node *inner =
      (struct super_node *)calloc(1, sizeof(struct super_node));
  inner->args = args;
  return ast_node_new(SUPER_NODE, inner);
}

struct ast_node *try_catch_node_new(struct ast_node *try,
                                    struct ast_node *catch, char *id) {
  struct try_catch_node *inner =
      (struct try_catch_node *)calloc(1, sizeof(struct try_catch_node));
  inner->try = try;
  inner->catch = catch;
  inner->id = id;
  return ast_node_new(TRY_CATCH_NODE, inner);
}

struct ast_node *unary_op_node_new(unary_op_type_t type,
                                   struct ast_node *target) {
  struct unary_op_node *inner =
      (struct unary_op_node *)calloc(1, sizeof(struct unary_op_node));
  inner->type = type;
  inner->target = target;
  return ast_node_new(UNARY_OP_NODE, inner);
}

struct ast_node *while_node_new(struct ast_node *condition,
                                struct ast_node *body) {
  struct while_node *inner =
      (struct while_node *)calloc(1, sizeof(struct while_node));
  inner->condition = condition;
  inner->body = body;
  return ast_node_new(WHILE_NODE, inner);
}

static void vec_ast_node_free(struct vec *);
static void array_decl_node_free(struct array_decl_node *);
static void attrib_node_free(struct attrib_node *);
static void bin_op_node_free(struct bin_op_node *);
static void class_decl_node_free(struct class_decl_node *);
static void code_block_node_free(struct code_block_node *);
static void expr_stmt_node_free(struct expr_stmt_node *);
static void for_node_free(struct for_node *);
static void foreach_node_free(struct foreach_node *);
static void func_decl_node_free(struct func_decl_node *);
static void id_node_free(struct id_node *);
static void if_node_free(struct if_node *);
static void import_node_free(struct import_node *);
static void invoke_node_free(struct invoke_node *);
static void obj_decl_node_free(struct obj_decl_node *);
static void raise_node_free(struct raise_node *);
static void return_node_free(struct return_node *);
static void string_node_free(struct string_node *);
static void subscript_node_free(struct subscript_node *);
static void super_node_free(struct super_node *);
static void try_catch_node_free(struct try_catch_node *);
static void unary_op_node_free(struct unary_op_node *);
static void while_node_free(struct while_node *);

void ast_node_free(struct ast_node *node) {
  if (node == NULL) return;
  switch (node->type) {
    case ARRAY_DECL_NODE:
      array_decl_node_free(node->inner);
      break;
    case ATTRIB_NODE:
      attrib_node_free(node->inner);
      break;
    case BIN_OP_NODE:
      bin_op_node_free(node->inner);
      break;
    case CLASS_DECL_NODE:
      class_decl_node_free(node->inner);
      break;
    case CODE_BLOCK_NODE:
      code_block_node_free(node->inner);
      break;
    case EXPR_STMT_NODE:
      expr_stmt_node_free(node->inner);
      break;
    case FOR_NODE:
      for_node_free(node->inner);
      break;
    case FOREACH_NODE:
      foreach_node_free(node->inner);
      break;
    case FUNC_DECL_NODE:
      func_decl_node_free(node->inner);
      break;
    case ID_NODE:
      id_node_free(node->inner);
      break;
    case IF_NODE:
      if_node_free(node->inner);
      break;
    case IMPORT_NODE:
      import_node_free(node->inner);
      break;
    case INVOKE_NODE:
      invoke_node_free(node->inner);
      break;
    case OBJ_DECL_NODE:
      obj_decl_node_free(node->inner);
      break;
    case RAISE_NODE:
      raise_node_free(node->inner);
      break;
    case RETURN_NODE:
      return_node_free(node->inner);
      break;
    case STRING_NODE:
      string_node_free(node->inner);
      break;
    case SUBSCRIPT_NODE:
      subscript_node_free(node->inner);
      break;
    case SUPER_NODE:
      super_node_free(node->inner);
      break;
    case TRY_CATCH_NODE:
      try_catch_node_free(node->inner);
      break;
    case UNARY_OP_NODE:
      unary_op_node_free(node->inner);
      break;
    case WHILE_NODE:
      while_node_free(node->inner);
      break;
    default:
      break;
  }
  free(node->inner);
  free(node);
}

static void vec_ast_node_free(struct vec *vec) {
  for (int i = 0; i < vec->len; i++) ast_node_free(vec_get(vec, i));
  vec_free(vec);
}

static void array_decl_node_free(struct array_decl_node *node) {
  vec_ast_node_free(node->values);
}

static void attrib_node_free(struct attrib_node *node) {
  free(node->attrib);
  ast_node_free(node->target);
}

static void bin_op_node_free(struct bin_op_node *node) {
  ast_node_free(node->left);
  ast_node_free(node->right);
}

static void class_decl_node_free(struct class_decl_node *node) {
  ast_node_free(node->body);
  ast_node_free(node->extends);
  free(node->name);
}

static void code_block_node_free(struct code_block_node *node) {
  vec_ast_node_free(node->children);
}

static void expr_stmt_node_free(struct expr_stmt_node *node) {
  ast_node_free(node->expr);
}

static void for_node_free(struct for_node *node) {
  ast_node_free(node->initial);
  ast_node_free(node->condition);
  ast_node_free(node->repeated);
  ast_node_free(node->body);
}

static void foreach_node_free(struct foreach_node *node) {
  free(node->id);
  ast_node_free(node->target);
  ast_node_free(node->body);
}

static void func_decl_node_free(struct func_decl_node *node) {
  free(node->name);
  ast_node_free(node->ret_type);
  ast_node_free(node->body);
}

static void id_node_free(struct id_node *node) { free(node->id); }

static void if_node_free(struct if_node *node) {
  ast_node_free(node->predicate);
  ast_node_free(node->body);
  ast_node_free(node->else_body);
}

static void import_node_free(struct import_node *node) {
  ast_node_free(node->target);
}

static void invoke_node_free(struct invoke_node *node) {
  ast_node_free(node->target);
  vec_ast_node_free(node->args);
}

static void obj_decl_node_free(struct obj_decl_node *node) {
  vec_ast_node_free(node->values);
}

static void raise_node_free(struct raise_node *node) {
  ast_node_free(node->value);
}

static void return_node_free(struct return_node *node) {
  ast_node_free(node->value);
}

static void string_node_free(struct string_node *node) { free(node->value); }

static void subscript_node_free(struct subscript_node *node) {
  ast_node_free(node->key);
  ast_node_free(node->target);
}

static void super_node_free(struct super_node *node) {
  vec_ast_node_free(node->args);
}

static void try_catch_node_free(struct try_catch_node *node) {
  ast_node_free(node->try);
  ast_node_free(node->catch);
  free(node->id);
}

static void unary_op_node_free(struct unary_op_node *node) {
  ast_node_free(node->target);
}

static void while_node_free(struct while_node *node) {
  ast_node_free(node->condition);
  ast_node_free(node->body);
}