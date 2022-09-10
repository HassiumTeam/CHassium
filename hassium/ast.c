#include <ast.h>

struct ast_node *ast_node_new(ast_type_t type, void *inner,
                              struct sourcepos *sourcepos) {
  struct ast_node *node = (struct ast_node *)malloc(sizeof(struct ast_node));
  node->type = type;
  node->inner = inner;
  node->sourcepos = sourcepos_inc_ref(sourcepos);
  return node;
}

struct ast_node *array_decl_node_new(struct vec *values,
                                     struct sourcepos *sourcepos) {
  struct array_decl_node *inner =
      (struct array_decl_node *)malloc(sizeof(struct array_decl_node));
  inner->values = values;
  return ast_node_new(ARRAY_DECL_NODE, inner, sourcepos);
}

struct ast_node *attrib_node_new(struct ast_node *target, char *attrib,
                                 struct sourcepos *sourcepos) {
  struct attrib_node *inner =
      (struct attrib_node *)malloc(sizeof(struct attrib_node));
  inner->target = target;
  inner->attrib = attrib;
  return ast_node_new(ATTRIB_NODE, inner, sourcepos);
}

struct ast_node *bin_op_node_new(bin_op_type_t type, struct ast_node *left,
                                 struct ast_node *right, bool for_switch,
                                 struct sourcepos *sourcepos) {
  struct bin_op_node *inner =
      (struct bin_op_node *)malloc(sizeof(struct bin_op_node));
  inner->type = type;
  inner->left = left;
  inner->right = right;
  inner->for_switch = for_switch;
  return ast_node_new(BIN_OP_NODE, inner, sourcepos);
}

struct ast_node *class_decl_node_new(char *name, struct ast_node *extends,
                                     struct ast_node *body, struct vec *impls,
                                     struct sourcepos *sourcepos) {
  struct class_decl_node *inner =
      (struct class_decl_node *)malloc(sizeof(struct class_decl_node));
  inner->name = name;
  inner->extends = extends;
  inner->body = body;
  inner->impls = impls;

  return ast_node_new(CLASS_DECL_NODE, inner, sourcepos);
}

struct ast_node *code_block_node_new(struct vec *children,
                                     struct sourcepos *sourcepos) {
  struct code_block_node *inner =
      (struct code_block_node *)malloc(sizeof(struct code_block_node));
  inner->children = children;

  return ast_node_new(CODE_BLOCK_NODE, inner, sourcepos);
}

struct ast_node *delete_node_new(struct ast_node *target,
                                 struct sourcepos *sourcepos) {
  struct delete_node *inner =
      (struct delete_node *)malloc(sizeof(struct delete_node));
  inner->target = target;

  return ast_node_new(DELETE_NODE, inner, sourcepos);
}

struct ast_node *do_while_node_new(struct ast_node *body,
                                   struct ast_node *condition,
                                   struct sourcepos *sourcepos) {
  struct while_node *inner =
      (struct while_node *)malloc(sizeof(struct while_node));
  inner->condition = condition;
  inner->body = body;

  return ast_node_new(DO_WHILE_NODE, inner, sourcepos);
}

struct ast_node *expr_stmt_node_new(struct ast_node *expr,
                                    struct sourcepos *sourcepos) {
  struct expr_stmt_node *inner =
      (struct expr_stmt_node *)malloc(sizeof(struct expr_stmt_node));
  inner->expr = expr;

  return ast_node_new(EXPR_STMT_NODE, inner, sourcepos);
}

struct ast_node *for_node_new(struct ast_node *initial,
                              struct ast_node *condition,
                              struct ast_node *repeated, struct ast_node *body,
                              struct sourcepos *sourcepos) {
  struct for_node *inner = (struct for_node *)malloc(sizeof(struct for_node));
  inner->initial = initial;
  inner->condition = condition;
  inner->repeated = repeated;
  inner->body = body;

  return ast_node_new(FOR_NODE, inner, sourcepos);
}

struct ast_node *foreach_node_new(char *id, struct ast_node *target,
                                  struct ast_node *body,
                                  struct sourcepos *sourcepos) {
  struct foreach_node *inner =
      (struct foreach_node *)malloc(sizeof(struct foreach_node));
  inner->id = id;
  inner->target = target;
  inner->body = body;

  return ast_node_new(FOREACH_NODE, inner, sourcepos);
}

struct ast_node *func_decl_node_new(char *name, struct vec *params,
                                    struct ast_node *ret_type,
                                    struct ast_node *body,
                                    struct sourcepos *sourcepos) {
  struct func_decl_node *inner =
      (struct func_decl_node *)malloc(sizeof(struct func_decl_node));
  inner->name = name;
  inner->params = params;
  inner->ret_type = ret_type;
  inner->body = body;

  return ast_node_new(FUNC_DECL_NODE, inner, sourcepos);
}

struct ast_node *id_node_new(char *id, struct ast_node *type,
                             struct sourcepos *sourcepos) {
  struct id_node *inner = (struct id_node *)malloc(sizeof(struct id_node));
  inner->id = id;
  inner->type = type;

  return ast_node_new(ID_NODE, inner, sourcepos);
}

struct ast_node *if_node_new(struct ast_node *predicate, struct ast_node *body,
                             struct ast_node *else_body,
                             struct sourcepos *sourcepos) {
  struct if_node *inner = (struct if_node *)malloc(sizeof(struct if_node));
  inner->predicate = predicate;
  inner->body = body;
  inner->else_body = else_body;

  return ast_node_new(IF_NODE, inner, sourcepos);
}

struct ast_node *import_node_new(struct vec *imports, struct vec *from,
                                 struct sourcepos *sourcepos) {
  struct import_node *inner =
      (struct import_node *)malloc(sizeof(struct import_node));
  inner->imports = imports;
  inner->from = from;

  return ast_node_new(IMPORT_NODE, inner, sourcepos);
}

struct ast_node *invoke_node_new(struct ast_node *target, struct vec *args,
                                 struct sourcepos *sourcepos) {
  struct invoke_node *inner =
      (struct invoke_node *)malloc(sizeof(struct invoke_node));
  inner->target = target;
  inner->args = args;

  return ast_node_new(INVOKE_NODE, inner, sourcepos);
}

struct ast_node *num_node_new(bool is_float, int val_int, float val_float,
                              struct sourcepos *sourcepos) {
  struct num_node *inner = (struct num_node *)malloc(sizeof(struct num_node));
  inner->is_float = is_float;
  inner->val_int = val_int;
  inner->val_float = val_float;

  return ast_node_new(NUM_NODE, inner, sourcepos);
}

struct ast_node *obj_decl_node_new(struct vec *keys, struct vec *values,
                                   struct sourcepos *sourcepos) {
  struct obj_decl_node *inner =
      (struct obj_decl_node *)malloc(sizeof(struct obj_decl_node));
  inner->keys = keys;
  inner->values = values;

  return ast_node_new(OBJ_DECL_NODE, inner, sourcepos);
}

struct ast_node *raise_node_new(struct ast_node *value,
                                struct sourcepos *sourcepos) {
  struct raise_node *inner =
      (struct raise_node *)malloc(sizeof(struct raise_node));
  inner->value = value;

  return ast_node_new(RAISE_NODE, inner, sourcepos);
}

struct ast_node *return_node_new(struct ast_node *value,
                                 struct sourcepos *sourcepos) {
  struct return_node *inner =
      (struct return_node *)malloc(sizeof(struct return_node));
  inner->value = value;

  return ast_node_new(RETURN_NODE, inner, sourcepos);
}

struct ast_node *slice_node_new(struct ast_node *target, struct ast_node *start,
                                struct ast_node *end,
                                struct sourcepos *sourcepos) {
  struct slice_node *inner = malloc(sizeof(struct slice_node));
  inner->target = target;
  inner->start = start;
  inner->end = end;

  return ast_node_new(SLICE_NODE, inner, sourcepos);
}

struct ast_node *string_node_new(char *value, struct sourcepos *sourcepos) {
  struct string_node *inner =
      (struct string_node *)malloc(sizeof(struct string_node));
  inner->value = value;

  return ast_node_new(STRING_NODE, inner, sourcepos);
}

struct ast_node *switch_node_new(struct ast_node *target, struct vec *cases,
                                 struct vec *case_bodies,
                                 struct ast_node *default_case,
                                 struct sourcepos *sourcepos) {
  struct switch_node *inner = malloc(sizeof(struct switch_node));
  inner->target = target;
  inner->cases = cases;
  inner->case_bodies = case_bodies;
  inner->default_case = default_case;

  return ast_node_new(SWITCH_NODE, inner, sourcepos);
}

struct ast_node *subscript_node_new(struct ast_node *target,
                                    struct ast_node *key,
                                    struct sourcepos *sourcepos) {
  struct subscript_node *inner =
      (struct subscript_node *)malloc(sizeof(struct subscript_node));
  inner->target = target;
  inner->key = key;
  return ast_node_new(SUBSCRIPT_NODE, inner, sourcepos);
}

struct ast_node *super_node_new(struct vec *args, struct sourcepos *sourcepos) {
  struct super_node *inner =
      (struct super_node *)malloc(sizeof(struct super_node));
  inner->args = args;
  return ast_node_new(SUPER_NODE, inner, sourcepos);
}

struct ast_node *try_catch_node_new(struct ast_node *try,
                                    struct ast_node *catch, char *id,
                                    struct sourcepos *sourcepos) {
  struct try_catch_node *inner =
      (struct try_catch_node *)malloc(sizeof(struct try_catch_node));
  inner->try = try;
  inner->catch = catch;
  inner->id = id;
  return ast_node_new(TRY_CATCH_NODE, inner, sourcepos);
}

struct ast_node *unary_op_node_new(unary_op_type_t type,
                                   struct ast_node *target, bool for_switch,
                                   struct sourcepos *sourcepos) {
  struct unary_op_node *inner =
      (struct unary_op_node *)malloc(sizeof(struct unary_op_node));
  inner->type = type;
  inner->target = target;
  inner->for_switch = for_switch;
  return ast_node_new(UNARY_OP_NODE, inner, sourcepos);
}

struct ast_node *while_node_new(struct ast_node *condition,
                                struct ast_node *body,
                                struct ast_node *else_body,
                                struct sourcepos *sourcepos) {
  struct while_node *inner =
      (struct while_node *)malloc(sizeof(struct while_node));
  inner->condition = condition;
  inner->body = body;
  inner->else_body = else_body;

  return ast_node_new(WHILE_NODE, inner, sourcepos);
}

static void vec_ast_node_free(struct vec *);
static void array_decl_node_free(struct array_decl_node *);
static void attrib_node_free(struct attrib_node *);
static void bin_op_node_free(struct bin_op_node *);
static void class_decl_node_free(struct class_decl_node *);
static void code_block_node_free(struct code_block_node *);
static void delete_node_free(struct delete_node *);
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
static void slice_node_free(struct slice_node *);
static void string_node_free(struct string_node *);
static void switch_node_free(struct switch_node *);
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
    case DELETE_NODE:
      delete_node_free(node->inner);
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
    case SLICE_NODE:
      slice_node_free(node->inner);
      break;
    case STRING_NODE:
      string_node_free(node->inner);
      break;
    case SWITCH_NODE:
      switch_node_free(node->inner);
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
    case DO_WHILE_NODE:
      while_node_free(node->inner);
      break;
    default:
      break;
  }
  sourcepos_dec_ref(node->sourcepos);
  free(node->inner);
  free(node);
}

static void vec_ast_node_free(struct vec *vec) {
  for (int i = 0; i < vec->len; ++i) ast_node_free(vec_get(vec, i));
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
  // This will be freed when visiting node->right
  if (node->type != BIN_OP_ASSIGN_SHORT) {
    ast_node_free(node->left);
  }
  ast_node_free(node->right);
}

static void class_decl_node_free(struct class_decl_node *node) {
  ast_node_free(node->body);
  ast_node_free(node->extends);
  vec_ast_node_free(node->impls);
  free(node->name);
}

static void code_block_node_free(struct code_block_node *node) {
  vec_ast_node_free(node->children);
}

static void delete_node_free(struct delete_node *node) {
  ast_node_free(node->target);
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
  vec_ast_node_free(node->params);
}

static void id_node_free(struct id_node *node) {
  free(node->id);
  if (node->type != NULL) ast_node_free(node->type);
}

static void if_node_free(struct if_node *node) {
  ast_node_free(node->predicate);
  ast_node_free(node->body);
  ast_node_free(node->else_body);
}

static void import_node_free(struct import_node *node) {
  vec_free_deep(node->from);
}

static void invoke_node_free(struct invoke_node *node) {
  ast_node_free(node->target);
  vec_ast_node_free(node->args);
}

static void obj_decl_node_free(struct obj_decl_node *node) {
  vec_free_deep(node->keys);
  vec_ast_node_free(node->values);
}

static void raise_node_free(struct raise_node *node) {
  ast_node_free(node->value);
}

static void return_node_free(struct return_node *node) {
  ast_node_free(node->value);
}

static void slice_node_free(struct slice_node *node) {
  ast_node_free(node->target);
  ast_node_free(node->start);
  ast_node_free(node->end);
}

static void string_node_free(struct string_node *node) { free(node->value); }

static void switch_node_free(struct switch_node *node) {
  ast_node_free(node->target);
  vec_ast_node_free(node->cases);
  vec_ast_node_free(node->case_bodies);
  ast_node_free(node->default_case);
}

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
  ast_node_free(node->else_body);
}