#include <emit.h>

struct emit {
  struct code_obj *code_obj;
};

static void visit_ast_node(struct emit *, struct ast_node *);
static void visit_array_decl_node(struct emit *, struct array_decl_node *);
static void visit_attrib_node(struct emit *, struct attrib_node *);
static void visit_bin_op_node(struct emit *, struct bin_op_node *);
static void visit_class_decl_node(struct emit *, struct class_decl_node *);
static void visit_code_block_node(struct emit *, struct code_block_node *);
static void visit_expr_stmt_node(struct emit *, struct expr_stmt_node *);
static void visit_for_node(struct emit *, struct for_node *);
static void visit_foreach_node(struct emit *, struct foreach_node *);
static void visit_func_decl_node(struct emit *, struct func_decl_node *);
static void visit_id_node(struct emit *, struct id_node *);
static void visit_if_node(struct emit *, struct if_node *);
static void visit_import_node(struct emit *, struct import_node *);
static void visit_invoke_node(struct emit *, struct invoke_node *);
static void visit_num_node(struct emit *, struct num_node *);
static void visit_obj_decl_node(struct emit *, struct obj_decl_node *);
static void visit_raise_node(struct emit *, struct raise_node *);
static void visit_return_node(struct emit *, struct return_node *);
static void visit_string_node(struct emit *, struct string_node *);
static void visit_subscript_node(struct emit *, struct subscript_node *);
static void visit_super_node(struct emit *, struct super_node *);
static void visit_try_catch_node(struct emit *, struct try_catch_node *);
static void visit_unary_op_node(struct emit *, struct unary_op_node *);
static void visit_while_node(struct emit *, struct while_node *);

static struct vm_inst *vm_inst_new(vm_inst_t, void *);
static struct vm_inst *bin_op_inst_new(bin_op_type_t);
static struct vm_inst *build_array_inst_new(int);
static struct vm_inst *build_class_inst_new(struct code_obj *, bool);
static struct vm_inst *build_func_inst_new(struct code_obj *, struct vec *,
                                           bool);
static struct vm_inst *build_obj_inst_new(struct vec *);
static struct vm_inst *invoke_inst_new(int);
static struct vm_inst *iter_next_inst_new(char *);
static struct vm_inst *jump_inst_new(int);
static struct vm_inst *jump_if_false_inst_new(int);
static struct vm_inst *jump_if_full_inst_new(int);
static struct vm_inst *load_attrib_inst_new(char *);
static struct vm_inst *load_id_inst_new(char *);
static struct vm_inst *load_const_inst_new(int);
static struct vm_inst *store_attrib_inst_new(char *);
static struct vm_inst *store_id_inst_new(char *);
static struct vm_inst *super_inst_new(int);
static struct vm_inst *unary_op_inst_new(unary_op_type_t);

static char *tmp_symbol();
static void add_inst(struct emit *, struct vm_inst *);
static int new_label();
static void place_label(struct emit *, int);

struct code_obj *compile_ast(struct ast_node *ast) {
  struct emit emit;
  emit.code_obj = code_obj_new(clone_str("__module__"));

  visit_ast_node(&emit, ast);

  return emit.code_obj;
}

static void visit_ast_node(struct emit *emit, struct ast_node *node) {
  switch (node->type) {
    case ARRAY_DECL_NODE:
      visit_array_decl_node(emit, node->inner);
      break;
    case ATTRIB_NODE:
      visit_attrib_node(emit, node->inner);
      break;
    case BIN_OP_NODE:
      visit_bin_op_node(emit, node->inner);
      break;
    case CLASS_DECL_NODE:
      visit_class_decl_node(emit, node->inner);
      break;
    case CODE_BLOCK_NODE:
      visit_code_block_node(emit, node->inner);
      break;
    case EXPR_STMT_NODE:
      visit_expr_stmt_node(emit, node->inner);
      break;
    case FOR_NODE:
      visit_for_node(emit, node->inner);
      break;
    case FOREACH_NODE:
      visit_foreach_node(emit, node->inner);
      break;
    case FUNC_DECL_NODE:
      visit_func_decl_node(emit, node->inner);
      break;
    case ID_NODE:
      visit_id_node(emit, node->inner);
      break;
    case IF_NODE:
      visit_if_node(emit, node->inner);
      break;
    case IMPORT_NODE:
      visit_import_node(emit, node->inner);
      break;
    case INVOKE_NODE:
      visit_invoke_node(emit, node->inner);
      break;
    case NUM_NODE:
      visit_num_node(emit, node->inner);
      break;
    case OBJ_DECL_NODE:
      visit_obj_decl_node(emit, node->inner);
      break;
    case RAISE_NODE:
      visit_raise_node(emit, node->inner);
      break;
    case RETURN_NODE:
      visit_return_node(emit, node->inner);
      break;
    case STRING_NODE:
      visit_string_node(emit, node->inner);
      break;
    case SUBSCRIPT_NODE:
      visit_subscript_node(emit, node->inner);
      break;
    case SUPER_NODE:
      visit_super_node(emit, node->inner);
      break;
    case TRY_CATCH_NODE:
      visit_try_catch_node(emit, node->inner);
      break;
    case UNARY_OP_NODE:
      visit_unary_op_node(emit, node->inner);
      break;
    case WHILE_NODE:
      visit_while_node(emit, node->inner);
      break;
  }
}

static void visit_array_decl_node(struct emit *emit,
                                  struct array_decl_node *node) {
  for (int i = 0; i < node->values->len; i++) {
    visit_ast_node(emit, vec_get(node->values, i));
  }
  add_inst(emit, build_array_inst_new(node->values->len));
}

static void visit_attrib_node(struct emit *emit, struct attrib_node *node) {
  visit_ast_node(emit, node->target);
  add_inst(emit, load_attrib_inst_new(clone_str(node->attrib)));
}

static void visit_bin_op_node(struct emit *emit, struct bin_op_node *node) {
  if (node->type == BIN_OP_ASSIGN || node->type == BIN_OP_ASSIGN_SHORT) {
    visit_ast_node(emit, node->right);
    switch (node->left->type) {
      case ATTRIB_NODE: {
        struct attrib_node *attrib_node = node->left->inner;
        visit_ast_node(emit, attrib_node->target);
        add_inst(emit, store_attrib_inst_new(clone_str(attrib_node->attrib)));
      } break;
      case ID_NODE: {
        struct id_node *id_node = node->left->inner;
        add_inst(emit, store_id_inst_new(clone_str(id_node->id)));
      } break;
    }
  } else {
    visit_ast_node(emit, node->left);
    visit_ast_node(emit, node->right);
    add_inst(emit, bin_op_inst_new(node->type));
  }
}

static void visit_class_decl_node(struct emit *emit,
                                  struct class_decl_node *node) {
  if (node->extends != NULL) visit_ast_node(emit, node->extends);
  struct code_obj *class = code_obj_new(clone_str(node->name));
  struct code_obj *swp = emit->code_obj;
  emit->code_obj = class;
  visit_ast_node(emit, node->body);
  emit->code_obj = swp;
  add_inst(emit, build_class_inst_new(class, node->extends != NULL));
}

static void visit_code_block_node(struct emit *emit,
                                  struct code_block_node *node) {
  add_inst(emit, vm_inst_new(INST_PUSH_FRAME, NULL));
  for (int i = 0; i < node->children->len; i++) {
    visit_ast_node(emit, vec_get(node->children, i));
  }
  add_inst(emit, vm_inst_new(INST_POP_FRAME, NULL));
}

static void visit_expr_stmt_node(struct emit *emit,
                                 struct expr_stmt_node *node) {
  visit_ast_node(emit, node->expr);
  add_inst(emit, vm_inst_new(INST_POP, NULL));
}

static void visit_for_node(struct emit *emit, struct for_node *node) {
  int body = new_label();
  int end = new_label();
  visit_ast_node(emit, node->initial);

  place_label(emit, body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_full_inst_new(end));
  visit_ast_node(emit, node->body);
  visit_ast_node(emit, node->repeated);
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
}

static void visit_foreach_node(struct emit *emit, struct foreach_node *node) {
  int body = new_label();
  int end = new_label();
  char *id = tmp_symbol();

  add_inst(emit, vm_inst_new(INST_PUSH_FRAME, NULL));
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_ITER, NULL));
  add_inst(emit, store_id_inst_new(id));
  add_inst(emit, vm_inst_new(INST_POP, NULL));

  place_label(emit, body);
  add_inst(emit, load_id_inst_new(clone_str(id)));
  add_inst(emit, jump_if_full_inst_new(end));  // note to decrement iter if full
  add_inst(emit, store_id_inst_new(clone_str(node->id)));
  add_inst(emit, vm_inst_new(INST_POP, NULL));
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
}

static void visit_func_decl_node(struct emit *emit,
                                 struct func_decl_node *node) {
  if (node->ret_type != NULL) visit_ast_node(emit, node->ret_type);
  struct code_obj *func = code_obj_new(clone_str(node->name));
  struct code_obj *swp = emit->code_obj;
  emit->code_obj = func;
  visit_ast_node(emit, node->body);
  emit->code_obj = swp;
  add_inst(emit,
           build_func_inst_new(func, node->params, node->ret_type != NULL));
  if (node->name != NULL) {
    add_inst(emit, store_id_inst_new(clone_str(func->name)));
    add_inst(emit, vm_inst_new(INST_POP, NULL));
  }
}

static void visit_id_node(struct emit *emit, struct id_node *node) {
  if (strcmp(node->id, "true") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_TRUE, NULL));
  } else if (strcmp(node->id, "false") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_FALSE, NULL));
  } else if (strcmp(node->id, "none") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, NULL));
  } else {
    add_inst(emit, load_id_inst_new(clone_str(node->id)));
  }
}

static void visit_if_node(struct emit *emit, struct if_node *node) {
  int else_ = new_label();
  int end = new_label();
  visit_ast_node(emit, node->predicate);
  add_inst(emit, jump_if_false_inst_new(else_));
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(end));
  place_label(emit, else_);
  if (node->else_body != NULL) {
    visit_ast_node(emit, node->else_body);
  }
  place_label(emit, end);
}

static void visit_import_node(struct emit *emit, struct import_node *node) {
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_IMPORT, NULL));
}

static void visit_invoke_node(struct emit *emit, struct invoke_node *node) {
  visit_ast_node(emit, node->target);
  for (int i = 0; i < node->args->len; i++)
    visit_ast_node(emit, vec_get(node->args, i));
  add_inst(emit, invoke_inst_new(node->args->len));
}

static void visit_num_node(struct emit *emit, struct num_node *node) {
  int idx = -1;
  for (int i = 0; i < emit->code_obj->consts->len; i++) {
    struct obj *const_ = vec_get(emit->code_obj->consts, i);
    if (const_->type == OBJ_NUM && obj_num_val(const_) == node->val_int &&
        obj_num_val(const_) == node->val_float) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    vec_push(emit->code_obj->consts,
             obj_inc_ref(
                 obj_num_new(node->is_float, node->val_int, node->val_float)));
    idx = emit->code_obj->consts->len - 1;
  }
  add_inst(emit, load_const_inst_new(idx));
}

static void visit_obj_decl_node(struct emit *emit, struct obj_decl_node *node) {
  for (int i = 0; i < node->values->len; i++) {
    visit_ast_node(emit, vec_get(node->values, i));
  }
  add_inst(emit, build_obj_inst_new(node->keys));
}

static void visit_raise_node(struct emit *emit, struct raise_node *node) {
  visit_ast_node(emit, node->value);
  add_inst(emit, vm_inst_new(INST_RAISE, NULL));
}

static void visit_return_node(struct emit *emit, struct return_node *node) {
  visit_ast_node(emit, node->value);
  add_inst(emit, vm_inst_new(INST_RETURN, NULL));
}

static void visit_string_node(struct emit *emit, struct string_node *node) {
  int idx = -1;
  for (int i = 0; i < emit->code_obj->consts->len; i++) {
    struct obj *const_ = vec_get(emit->code_obj->consts, i);
    if (const_->type == OBJ_STRING &&
        strcmp(obj_string_val(const_), node->value) == 0) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    vec_push(emit->code_obj->consts, obj_inc_ref(obj_string_new(node->value)));
    idx = emit->code_obj->consts->len - 1;
  }
  add_inst(emit, load_const_inst_new(idx));
}

static void visit_subscript_node(struct emit *emit,
                                 struct subscript_node *node) {
  visit_ast_node(emit, node->key);
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_LOAD_SUBSCRIPT, NULL));
}

static void visit_super_node(struct emit *emit, struct super_node *node) {
  for (int i = 0; i < node->args->len; i++)
    visit_ast_node(emit, vec_get(node->args, i));
  add_inst(emit, super_inst_new(node->args->len));
}

static void visit_try_catch_node(struct emit *emit,
                                 struct try_catch_node *node) {}

static void visit_unary_op_node(struct emit *emit, struct unary_op_node *node) {
  visit_ast_node(emit, node->target);
  add_inst(emit, unary_op_inst_new(node->type));
}

static void visit_while_node(struct emit *emit, struct while_node *node) {
  int body = new_label();
  int end = new_label();
  place_label(emit, body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(end));
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body));
  place_label(emit, end);
}

static int symbol_idx = 0;
static char *tmp_symbol() {
  char *sym = (char *)calloc(8, sizeof(char));
  sprintf(sym, "%d", symbol_idx++);
  return sym;
}

static void add_inst(struct emit *emit, struct vm_inst *inst) {
  vec_push(emit->code_obj->instructions, inst);
}

static int label_idx = 0;
static int new_label() { return label_idx++; }

static void place_label(struct emit *emit, int label) {
  intmap_insert(emit->code_obj->labels, label,
                emit->code_obj->instructions->len - 1);
}

static struct vm_inst *vm_inst_new(vm_inst_t type, void *inner) {
  struct vm_inst *inst = (struct vm_inst *)calloc(1, sizeof(struct vm_inst));
  inst->type = type;
  inst->inner = inner;
  return inst;
}

static struct vm_inst *bin_op_inst_new(bin_op_type_t type) {
  struct bin_op_inst *inner =
      (struct bin_op_inst *)calloc(1, sizeof(struct bin_op_inst));
  inner->type = type;
  return vm_inst_new(INST_BIN_OP, inner);
}

static struct vm_inst *build_array_inst_new(int count) {
  struct build_array_inst *inner =
      (struct build_array_inst *)malloc(sizeof(struct build_array_inst));
  inner->count = count;
  return vm_inst_new(INST_BUILD_ARRAY, inner);
}

static struct vm_inst *build_class_inst_new(struct code_obj *code_obj,
                                            bool extends) {
  struct build_class_inst *inner =
      (struct build_class_inst *)calloc(1, sizeof(struct build_class_inst));
  inner->code_obj = code_obj;
  inner->extends = extends;
  return vm_inst_new(INST_BUILD_CLASS, inner);
}

static struct vm_inst *build_func_inst_new(struct code_obj *code_obj,
                                           struct vec *params,
                                           bool with_return_type) {
  struct build_func_inst *inner =
      (struct build_func_inst *)calloc(1, sizeof(struct build_func_inst));
  inner->code_obj = code_obj;
  inner->params = params;
  inner->with_return_type = with_return_type;
  return vm_inst_new(INST_BUILD_FUNC, inner);
}

static struct vm_inst *build_obj_inst_new(struct vec *keys) {
  struct build_obj_inst *inner =
      (struct build_obj_inst *)malloc(sizeof(struct build_obj_inst));
  inner->keys = keys;
  return vm_inst_new(INST_BUILD_OBJ, inner);
}

static struct vm_inst *invoke_inst_new(int arg_count) {
  struct invoke_inst *inner =
      (struct invoke_inst *)calloc(1, sizeof(struct invoke_inst));
  inner->arg_count = arg_count;
  return vm_inst_new(INST_INVOKE, inner);
}

static struct vm_inst *jump_inst_new(int label) {
  struct jump_inst *inner =
      (struct jump_inst *)calloc(1, sizeof(struct jump_inst));
  inner->label = label;
  return vm_inst_new(INST_JUMP, inner);
}

static struct vm_inst *jump_if_false_inst_new(int label) {
  struct jump_inst *inner =
      (struct jump_inst *)calloc(1, sizeof(struct jump_inst));
  inner->label = label;
  return vm_inst_new(INST_JUMP_IF_FALSE, inner);
}

static struct vm_inst *jump_if_full_inst_new(int label) {
  struct jump_inst *inner =
      (struct jump_inst *)calloc(1, sizeof(struct jump_inst));
  inner->label = label;
  return vm_inst_new(INST_JUMP_IF_FULL, inner);
}

static struct vm_inst *load_attrib_inst_new(char *attrib) {
  struct load_attrib_inst *inner =
      (struct load_attrib_inst *)calloc(1, sizeof(struct load_attrib_inst));
  inner->attrib = attrib;
  return vm_inst_new(INST_LOAD_ATTRIB, inner);
}

static struct vm_inst *iter_next_inst_new(char *id) {
  struct iter_next_inst *inner =
      (struct iter_next_inst *)malloc(sizeof(struct iter_next_inst));
  inner->id = id;
  return vm_inst_new(INST_ITER_NEXT, inner);
}

static struct vm_inst *load_id_inst_new(char *id) {
  struct load_id_inst *inner =
      (struct load_id_inst *)calloc(1, sizeof(struct load_id_inst));
  inner->id = id;
  return vm_inst_new(INST_LOAD_ID, inner);
}

static struct vm_inst *load_const_inst_new(int idx) {
  struct load_const_inst *inner =
      (struct load_const_inst *)calloc(1, sizeof(struct load_const_inst));
  inner->idx = idx;
  return vm_inst_new(INST_LOAD_CONST, inner);
}

static struct vm_inst *store_attrib_inst_new(char *attrib) {
  struct store_attrib_inst *inner =
      (struct store_attrib_inst *)calloc(1, sizeof(struct store_attrib_inst));
  inner->attrib = attrib;
  return vm_inst_new(INST_STORE_ATTRIB, inner);
}

static struct vm_inst *store_id_inst_new(char *id) {
  struct store_id_inst *inner =
      (struct store_id_inst *)calloc(1, sizeof(struct store_id_inst));
  inner->id = id;
  return vm_inst_new(INST_STORE_ID, inner);
}

static struct vm_inst *super_inst_new(int arg_count) {
  struct super_inst *inner =
      (struct super_inst *)calloc(1, sizeof(struct super_inst));
  inner->arg_count = arg_count;
  return vm_inst_new(INST_SUPER, inner);
}

static struct vm_inst *unary_op_inst_new(unary_op_type_t type) {
  struct unary_op_inst *inner =
      (struct unary_op_inst *)calloc(1, sizeof(struct unary_op_inst));
  inner->type = type;
  return vm_inst_new(INST_UNARY_OP, inner);
}
