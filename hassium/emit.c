#include <emit.h>

struct emit {
  struct code_obj *code_obj;
  struct vec *symtable;
  struct ast_node *ret_type;
  struct code_obj *func;
  struct code_obj *class;
};

static void visit_ast_node(struct emit *, struct ast_node *);
static void visit_array_decl_node(struct emit *, struct array_decl_node *);
static void visit_attrib_node(struct emit *, struct attrib_node *);
static void visit_bin_op_node(struct emit *, struct bin_op_node *);
static void visit_break_node(struct emit *);
static void visit_class_decl_node(struct emit *, struct class_decl_node *);
static void visit_code_block_node(struct emit *, struct code_block_node *,
                                  bool);
static void visit_continue_node(struct emit *);
static void visit_delete_node(struct emit *, struct delete_node *);
static void visit_do_while_node(struct emit *, struct while_node *);
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
static void visit_proto_node(struct emit *, struct proto_node *);
static void visit_raise_node(struct emit *, struct raise_node *);
static void visit_return_node(struct emit *, struct return_node *);
static void visit_slice_node(struct emit *, struct slice_node *);
static void visit_string_node(struct emit *, struct string_node *);
static void visit_subscript_node(struct emit *, struct subscript_node *);
static void visit_super_node(struct emit *, struct super_node *);
static void visit_try_catch_node(struct emit *, struct try_catch_node *);
static void visit_unary_op_node(struct emit *, struct unary_op_node *);
static void visit_while_node(struct emit *, struct while_node *);

static vm_inst_t vm_inst_new(uint16_t opcode, uint16_t opshort, uint32_t op);
static vm_inst_t bin_op_inst_new(bin_op_type_t);
static vm_inst_t build_array_inst_new(int);
static vm_inst_t build_class_inst_new(struct emit *, struct code_obj *, bool);
static vm_inst_t build_func_inst_new(struct emit *, struct code_obj *,
                                     struct vec *, bool);
static vm_inst_t build_handler_inst_new(struct emit *, struct code_obj *);
static vm_inst_t build_obj_inst_new(struct emit *, struct vec *);
static vm_inst_t import_inst_new(struct emit *, struct vec *,
                                 struct code_obj *);
static vm_inst_t invoke_inst_new(int);
static vm_inst_t iter_next_inst_new(struct emit *, char *);
static vm_inst_t jump_inst_new(int);
static vm_inst_t jump_if_false_inst_new(int);
static vm_inst_t jump_if_full_inst_new(int);
static vm_inst_t load_attrib_inst_new(struct emit *, char *);
static vm_inst_t load_fast_inst_new(int);
static vm_inst_t load_id_inst_new(struct emit *, char *);
static vm_inst_t load_const_inst_new(int);
static vm_inst_t store_attrib_inst_new(struct emit *, char *);
static vm_inst_t store_fast_inst_new(int);
static vm_inst_t store_id_inst_new(struct emit *, char *);
static vm_inst_t super_inst_new(int);
static vm_inst_t unary_op_inst_new(unary_op_type_t);

struct labels_ctx {
  int break_count;
  int cont_count;
};
static struct labels_ctx save_labels(struct emit *);
static void restore_labels(struct emit *, struct labels_ctx);
static int next_sym_idx = 0;
static char *tmp_symbol();
static void add_inst(struct emit *, vm_inst_t);
static int new_label();
static void place_label(struct emit *, int);

static void enter_scope(struct emit *);
static void leave_scope(struct emit *);
static int get_symbol(struct emit *, char *);
static bool has_symbol(struct emit *, char *);
static int handle_symbol(struct emit *, char *);

struct code_obj *compile_ast(struct ast_node *ast) {
  struct emit emit;
  emit.code_obj = code_obj_new(clone_str("__module__"));
  emit.ret_type = NULL;
  emit.symtable = vec_new();
  emit.func = NULL;
  emit.class = NULL;

  visit_code_block_node(&emit, ast->inner, true);
  add_inst(&emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  add_inst(&emit, vm_inst_new(INST_RETURN, 0, 0));

  vec_free(emit.symtable);
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
    case BREAK_NODE:
      visit_break_node(emit);
      break;
    case CLASS_DECL_NODE:
      visit_class_decl_node(emit, node->inner);
      break;
    case CODE_BLOCK_NODE:
      visit_code_block_node(emit, node->inner, true);
      break;
    case CONTINUE_NODE:
      visit_continue_node(emit);
      break;
    case DELETE_NODE:
      visit_delete_node(emit, node->inner);
      break;
    case DO_WHILE_NODE:
      visit_do_while_node(emit, node->inner);
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
    case PROTO_NODE:
      visit_proto_node(emit, node->inner);
      break;
    case RAISE_NODE:
      visit_raise_node(emit, node->inner);
      break;
    case RETURN_NODE:
      visit_return_node(emit, node->inner);
      break;
    case SLICE_NODE:
      visit_slice_node(emit, node->inner);
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
  add_inst(emit, load_attrib_inst_new(emit, clone_str(node->attrib)));
}

static void visit_bin_op_node(struct emit *emit, struct bin_op_node *node) {
  if (node->type == BIN_OP_ASSIGN || node->type == BIN_OP_ASSIGN_SHORT) {
    visit_ast_node(emit, node->right);
    switch (node->left->type) {
      case ATTRIB_NODE: {
        struct attrib_node *attrib_node = node->left->inner;
        visit_ast_node(emit, attrib_node->target);
        add_inst(emit,
                 store_attrib_inst_new(emit, clone_str(attrib_node->attrib)));
      } break;
      case ID_NODE: {
        struct id_node *id_node = node->left->inner;
        add_inst(emit, store_fast_inst_new(handle_symbol(emit, id_node->id)));
      } break;
      case SUBSCRIPT_NODE: {
        struct subscript_node *subscript_node = node->left->inner;
        visit_ast_node(emit, subscript_node->key);
        visit_ast_node(emit, subscript_node->target);
        add_inst(emit, vm_inst_new(INST_STORE_SUBSCRIPT, 0, 0));
      }
    }
  } else {
    visit_ast_node(emit, node->left);
    visit_ast_node(emit, node->right);
    add_inst(emit, bin_op_inst_new(node->type));
  }
}

static void visit_break_node(struct emit *emit) {
  if (emit->code_obj->break_labels->len <= 0) {
    printf("Invalid use of break\n");
    exit(-1);
  }
  add_inst(emit,
           jump_inst_new((uintptr_t)vec_pop(emit->code_obj->break_labels)));
}

static void visit_class_decl_node(struct emit *emit,
                                  struct class_decl_node *node) {
  if (node->extends != NULL) visit_ast_node(emit, node->extends);
  struct code_obj *class = code_obj_new(clone_str(node->name));

  int symbol_idx_swp = next_sym_idx;
  if (emit->class == NULL) {
    next_sym_idx = 0;
    emit->class = class;
  }

  struct code_obj *swp = emit->code_obj;
  emit->code_obj = class;
  visit_ast_node(emit, node->body);
  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0));
  emit->code_obj = swp;

  if (emit->class == class) {
    emit->class = NULL;
    next_sym_idx = symbol_idx_swp;
  }

  add_inst(emit, build_class_inst_new(emit, class, node->extends != NULL));
}

static void visit_code_block_node(struct emit *emit,
                                  struct code_block_node *node,
                                  bool new_scope) {
  if (new_scope) {
    enter_scope(emit);
  }
  for (int i = 0; i < node->children->len; i++) {
    visit_ast_node(emit, vec_get(node->children, i));
  }
  if (new_scope) {
    leave_scope(emit);
  }
}

static void visit_continue_node(struct emit *emit) {
  if (emit->code_obj->cont_labels->len <= 0) {
    printf("Invalid use of continue\n");
    exit(-1);
  }
  add_inst(emit,
           jump_inst_new((uintptr_t)vec_pop(emit->code_obj->cont_labels)));
}

static void visit_delete_node(struct emit *emit, struct delete_node *node) {
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_DELETE, 0, 0));
}

static void visit_do_while_node(struct emit *emit, struct while_node *node) {
  int body = new_label();
  int end = new_label();

  struct labels_ctx labels = {
      .break_count = emit->code_obj->break_labels->len,
      .cont_count = emit->code_obj->cont_labels->len,
  };
  vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)end);
  vec_push(emit->code_obj->cont_labels, (void *)(uintptr_t)body);

  place_label(emit, body);
  visit_ast_node(emit, node->body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(end));
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void visit_expr_stmt_node(struct emit *emit,
                                 struct expr_stmt_node *node) {
  visit_ast_node(emit, node->expr);
  add_inst(emit, vm_inst_new(INST_POP, 0, 0));
}

static void visit_for_node(struct emit *emit, struct for_node *node) {
  int body = new_label();
  int end = new_label();
  struct labels_ctx labels = {
      .break_count = emit->code_obj->break_labels->len,
      .cont_count = emit->code_obj->cont_labels->len,
  };
  vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)end);
  vec_push(emit->code_obj->cont_labels, (void *)(uintptr_t)body);

  visit_ast_node(emit, node->initial);

  place_label(emit, body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(end));
  visit_ast_node(emit, node->body);
  visit_ast_node(emit, node->repeated);
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void visit_foreach_node(struct emit *emit, struct foreach_node *node) {
  int body = new_label();
  int end = new_label();
  char *id = tmp_symbol();
  struct labels_ctx labels = {
      .break_count = emit->code_obj->break_labels->len,
      .cont_count = emit->code_obj->cont_labels->len,
  };
  vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)end);
  vec_push(emit->code_obj->cont_labels, (void *)(uintptr_t)body);

  enter_scope(emit);
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_ITER, 0, 0));
  add_inst(emit, store_fast_inst_new(handle_symbol(emit, id)));
  add_inst(emit, vm_inst_new(INST_POP, 0, 0));

  place_label(emit, body);
  add_inst(emit, load_fast_inst_new(get_symbol(emit, id)));
  add_inst(emit, jump_if_full_inst_new(end));  // note to decrement iter if full
  add_inst(emit, store_fast_inst_new(handle_symbol(emit, node->id)));
  add_inst(emit, vm_inst_new(INST_POP, 0, 0));
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
  leave_scope(emit);
  restore_labels(emit, labels);

  free(id);
}

static void visit_func_decl_node(struct emit *emit,
                                 struct func_decl_node *node) {
  struct code_obj *func = code_obj_new(clone_str(node->name));
  struct code_obj *swp = emit->code_obj;
  bool closure = node->name == NULL;
  emit->code_obj = func;
  if (node->ret_type != NULL) {
    emit->ret_type = node->ret_type;
  }

  int symbol_idx_swp = next_sym_idx;
  if (emit->func == NULL && !closure) {
    next_sym_idx = 0;
    emit->func = func;
  }

  enter_scope(emit);

  struct vec *func_params = vec_new();
  for (int i = 0; i < node->params->len; i++) {
    struct ast_node *id_ast = vec_get(node->params, i);
    struct id_node *id_node = id_ast->inner;
    int sym = handle_symbol(emit, id_node->id);
    vec_push(func_params, (void *)(uintptr_t)sym);
    if (id_node->type != NULL) {
      visit_ast_node(emit, id_node->type);
      add_inst(emit, vm_inst_new(INST_TYPECHECK_FAST, 0, (uintptr_t)sym));
    }
  }

  if (node->body->type == CODE_BLOCK_NODE) {
    visit_code_block_node(emit, node->body->inner, false);
  } else {
    visit_ast_node(emit, node->body);
  }
  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0));

  leave_scope(emit);

  emit->code_obj = swp;
  emit->ret_type = NULL;
  if (emit->func == func) {
    emit->func = NULL;
    next_sym_idx = symbol_idx_swp;
  } else {
    emit->code_obj->locals += func->locals;
    closure = true;
  }

  add_inst(emit, build_func_inst_new(emit, func, func_params, closure));

  if (node->name != NULL) {
    if (emit->symtable->len > 1) {
      add_inst(emit, store_fast_inst_new(handle_symbol(emit, node->name)));
    } else {
      add_inst(emit, store_id_inst_new(emit, clone_str(node->name)));
    }
    add_inst(emit, vm_inst_new(INST_POP, 0, 0));
  }
}

static void visit_id_node(struct emit *emit, struct id_node *node) {
  if (strcmp(node->id, "true") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_TRUE, 0, 0));
  } else if (strcmp(node->id, "false") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_FALSE, 0, 0));
  } else if (strcmp(node->id, "none") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  } else if (strcmp(node->id, "self") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_SELF, 0, 0));
  } else if (get_symbol(emit, node->id) != -1) {
    add_inst(emit, load_fast_inst_new(get_symbol(emit, node->id)));
  } else {
    add_inst(emit, load_id_inst_new(emit, clone_str(node->id)));
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
  struct code_obj *mod = compile_module_for_import(node->from);
  add_inst(emit, import_inst_new(emit, node->imports, mod));
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
  add_inst(emit, build_obj_inst_new(emit, node->keys));
}

static void visit_proto_node(struct emit *emit, struct proto_node *node) {
  struct proto_obj_ctx *proto_ctx = malloc(sizeof(struct proto_obj_ctx));
  proto_ctx->class_attribs = node->class_attribs;
  proto_ctx->instance_attribs = vec_new();

  for (int i = 0; i < node->instance_attribs->len; i++) {
    struct proto_node_instance_attrib *node_attrib =
        vec_get(node->instance_attribs, i);
    struct proto_instance_attrib *runtime_attrib =
        malloc(sizeof(struct proto_instance_attrib));
    runtime_attrib->name = clone_str(node_attrib->name);
    runtime_attrib->count = node_attrib->count;
    runtime_attrib->type = NULL;
  }
}

static void visit_raise_node(struct emit *emit, struct raise_node *node) {
  visit_ast_node(emit, node->value);
  add_inst(emit, vm_inst_new(INST_RAISE, 0, 0));
}

static void visit_return_node(struct emit *emit, struct return_node *node) {
  visit_ast_node(emit, node->value);
  if (emit->ret_type != NULL) {
    visit_ast_node(emit, emit->ret_type);
    add_inst(emit, vm_inst_new(INST_TYPECHECK, 0, 0));
  }
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0));
}

static void visit_slice_node(struct emit *emit, struct slice_node *node) {
  visit_ast_node(emit, node->target);
  if (node->start != NULL) {
    visit_ast_node(emit, node->start);
  } else {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  }
  if (node->end != NULL) {
    visit_ast_node(emit, node->end);
  } else {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  }
  add_inst(emit, vm_inst_new(INST_SLICE, 0, 0));
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
  add_inst(emit, vm_inst_new(INST_LOAD_SUBSCRIPT, 0, 0));
}

static void visit_super_node(struct emit *emit, struct super_node *node) {
  for (int i = 0; i < node->args->len; i++)
    visit_ast_node(emit, vec_get(node->args, i));
  add_inst(emit, super_inst_new(node->args->len));
}

static void visit_try_catch_node(struct emit *emit,
                                 struct try_catch_node *node) {
  struct code_obj *handler = code_obj_new(NULL);
  struct code_obj *swp = emit->code_obj;
  emit->code_obj = handler;

  enter_scope(emit);

  handler->params = vec_new();
  int sym = handle_symbol(emit, node->id);
  vec_push(handler->params, (void *)(uintptr_t)sym);
  if (node->catch->type == CODE_BLOCK_NODE) {
    visit_code_block_node(emit, node->catch->inner, false);
  } else {
    visit_ast_node(emit, node->catch);
  }
  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0));
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0));

  leave_scope(emit);
  emit->code_obj = swp;
  handler->parent = emit->code_obj;
  emit->code_obj->locals += handler->locals;

  int end = new_label();
  handler->caught_label = end;
  add_inst(emit, build_handler_inst_new(emit, handler));

  enter_scope(emit);
  visit_ast_node(emit, node->try);
  leave_scope(emit);

  add_inst(emit, vm_inst_new(INST_POP_HANDLER, 0, 0));
  place_label(emit, end);
}

static void visit_unary_op_node(struct emit *emit, struct unary_op_node *node) {
  visit_ast_node(emit, node->target);
  add_inst(emit, unary_op_inst_new(node->type));
}

static void visit_while_node(struct emit *emit, struct while_node *node) {
  int body = new_label();
  int end = new_label();

  struct labels_ctx labels = {
      .break_count = emit->code_obj->break_labels->len,
      .cont_count = emit->code_obj->cont_labels->len,
  };
  vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)end);
  vec_push(emit->code_obj->cont_labels, (void *)(uintptr_t)body);

  place_label(emit, body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(end));
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body));

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void restore_labels(struct emit *emit, struct labels_ctx labels_ctx) {
  for (int i = emit->code_obj->break_labels->len; i < labels_ctx.break_count;
       i++) {
    vec_pop(emit->code_obj->break_labels);
  }
  for (int i = emit->code_obj->cont_labels->len; i < labels_ctx.cont_count;
       i++) {
    vec_pop(emit->code_obj->cont_labels);
  }
}

static int tmp_sym_idx = 0;
static char *tmp_symbol() {
  char *sym = (char *)calloc(8, sizeof(char));
  sprintf(sym, "%d", tmp_sym_idx++);
  return sym;
}

static void add_inst(struct emit *emit, vm_inst_t inst) {
  vec_push(emit->code_obj->instructions, (void *)inst);
}

static int label_idx = 0;
static int new_label() { return label_idx++; }

static void place_label(struct emit *emit, int label) {
  intmap_insert(emit->code_obj->labels, label,
                emit->code_obj->instructions->len - 1);
}

static void enter_scope(struct emit *emit) {
  vec_push(emit->symtable, hashmap_create());
}

static void leave_scope(struct emit *emit) {
  hashmap_free(vec_pop(emit->symtable));
}

static int get_symbol(struct emit *emit, char *sym) {
  for (int i = emit->symtable->len - 1; i >= 0; --i) {
    uintptr_t idx;
    struct hashmap *map = vec_get(emit->symtable, i);
    if (hashmap_get(map, sym, strlen(sym), &idx)) {
      return (int)idx;
    }
  }
  return -1;
}

static int handle_symbol(struct emit *emit, char *sym) {
  int get = get_symbol(emit, sym);
  if (get != -1) return get;
  struct hashmap *map = vec_peek(emit->symtable);
  hashmap_set(map, sym, strlen(sym), next_sym_idx++);
  ++emit->code_obj->locals;
  return next_sym_idx - 1;
}

static vm_inst_t vm_inst_new(uint16_t opcode, uint16_t opshort, uint32_t op) {
  vm_inst_t inst = 0;
  inst |= ((uint64_t)opcode) << 48;
  inst |= ((uint64_t)opshort) << 32;
  inst |= op;

  return inst;
}

static vm_inst_t bin_op_inst_new(bin_op_type_t type) {
  return vm_inst_new(INST_BIN_OP, type, 0);
}

static vm_inst_t build_array_inst_new(int count) {
  return vm_inst_new(INST_BUILD_ARRAY, 0, count);
}

static vm_inst_t build_class_inst_new(struct emit *emit,
                                      struct code_obj *code_obj, bool extends) {
  vec_push(emit->code_obj->code_objs, code_obj);
  return vm_inst_new(INST_BUILD_CLASS, extends,
                     emit->code_obj->code_objs->len - 1);
}

static vm_inst_t build_func_inst_new(struct emit *emit,
                                     struct code_obj *code_obj,
                                     struct vec *params, bool closure) {
  code_obj->params = params;
  vec_push(emit->code_obj->code_objs, code_obj);
  return vm_inst_new(INST_BUILD_FUNC, closure,
                     emit->code_obj->code_objs->len - 1);
}

static vm_inst_t build_handler_inst_new(struct emit *emit,
                                        struct code_obj *handler) {
  vec_push(emit->code_obj->code_objs, handler);
  return vm_inst_new(INST_BUILD_HANDLER, 0, emit->code_obj->code_objs->len - 1);
}

static vm_inst_t build_obj_inst_new(struct emit *emit, struct vec *keys) {
  vec_push(emit->code_obj->vecs, keys);
  return vm_inst_new(INST_BUILD_OBJ, 0, emit->code_obj->vecs->len - 1);
}

static vm_inst_t import_inst_new(struct emit *emit, struct vec *imports,
                                 struct code_obj *mod) {
  vec_push(emit->code_obj->vecs, imports);
  vec_push(emit->code_obj->code_objs, mod);
  return vm_inst_new(INST_IMPORT, (uint16_t)emit->code_obj->vecs->len - 1,
                     emit->code_obj->code_objs->len - 1);
}

static vm_inst_t invoke_inst_new(int arg_count) {
  return vm_inst_new(INST_INVOKE, 0, arg_count);
}

static vm_inst_t iter_next_inst_new(struct emit *emit, char *id) {
  vec_push(emit->code_obj->strs, id);
  return vm_inst_new(INST_ITER_NEXT, 0, emit->code_obj->strs->len - 1);
}

static vm_inst_t jump_inst_new(int label) {
  return vm_inst_new(INST_JUMP, 0, label);
}

static vm_inst_t jump_if_false_inst_new(int label) {
  return vm_inst_new(INST_JUMP_IF_FALSE, 0, label);
}

static vm_inst_t jump_if_full_inst_new(int label) {
  return vm_inst_new(INST_JUMP_IF_FULL, 0, label);
}

static vm_inst_t load_attrib_inst_new(struct emit *emit, char *attrib) {
  vec_push(emit->code_obj->strs, attrib);
  return vm_inst_new(INST_LOAD_ATTRIB, 0, emit->code_obj->strs->len - 1);
}

static vm_inst_t load_fast_inst_new(int idx) {
  return vm_inst_new(INST_LOAD_FAST, 0, idx);
}

static vm_inst_t load_id_inst_new(struct emit *emit, char *id) {
  vec_push(emit->code_obj->strs, id);
  return vm_inst_new(INST_LOAD_ID, 0, emit->code_obj->strs->len - 1);
}

static vm_inst_t load_const_inst_new(int idx) {
  return vm_inst_new(INST_LOAD_CONST, 0, idx);
}

static vm_inst_t store_attrib_inst_new(struct emit *emit, char *attrib) {
  vec_push(emit->code_obj->strs, attrib);
  return vm_inst_new(INST_STORE_ATTRIB, 0, emit->code_obj->strs->len - 1);
}

static vm_inst_t store_fast_inst_new(int idx) {
  return vm_inst_new(INST_STORE_FAST, 0, idx);
}

static vm_inst_t store_id_inst_new(struct emit *emit, char *id) {
  vec_push(emit->code_obj->strs, id);
  return vm_inst_new(INST_STORE_ID, 0, emit->code_obj->strs->len - 1);
}

static vm_inst_t super_inst_new(int arg_count) {
  return vm_inst_new(INST_SUPER, 0, arg_count);
}

static vm_inst_t unary_op_inst_new(unary_op_type_t type) {
  return vm_inst_new(INST_UNARY_OP, type, 0);
}
