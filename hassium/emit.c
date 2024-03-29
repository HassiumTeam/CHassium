#include <emit.h>

struct emit {
  struct code_obj *code_obj;
  struct vec *symtable;
  struct ast_node *ret_type;
  struct code_obj *func;
  struct code_obj *class;
  struct vm *vm;
};

static void visit_ast_node(struct emit *, struct ast_node *);
static void visit_array_decl_node(struct emit *, struct array_decl_node *,
                                  struct sourcepos *);
static void visit_attrib_node(struct emit *, struct attrib_node *,
                              struct sourcepos *);
static void visit_bin_op_node(struct emit *, struct bin_op_node *,
                              struct sourcepos *);
static void visit_break_node(struct emit *, struct sourcepos *);
static void visit_class_decl_node(struct emit *, struct class_decl_node *,
                                  struct sourcepos *);
static void visit_code_block_node(struct emit *, struct code_block_node *,
                                  bool);
static void visit_continue_node(struct emit *, struct sourcepos *);
static void visit_delete_node(struct emit *, struct delete_node *,
                              struct sourcepos *);
static void visit_do_while_node(struct emit *, struct while_node *,
                                struct sourcepos *);
static void visit_expr_stmt_node(struct emit *, struct expr_stmt_node *,
                                 struct sourcepos *);
static void visit_for_node(struct emit *, struct for_node *,
                           struct sourcepos *);
static void visit_foreach_node(struct emit *, struct foreach_node *,
                               struct sourcepos *);
static void visit_func_decl_node(struct emit *, struct func_decl_node *,
                                 struct sourcepos *);
static void visit_id_node(struct emit *, struct id_node *, struct sourcepos *);
static void visit_if_node(struct emit *, struct if_node *, struct sourcepos *);
static void visit_import_node(struct emit *, struct import_node *,
                              struct sourcepos *);
static void visit_invoke_node(struct emit *, struct invoke_node *,
                              struct sourcepos *);
static void visit_num_node(struct emit *, struct num_node *,
                           struct sourcepos *);
static void visit_obj_decl_node(struct emit *, struct obj_decl_node *,
                                struct sourcepos *);
static void visit_raise_node(struct emit *, struct raise_node *,
                             struct sourcepos *);
static void visit_return_node(struct emit *, struct return_node *,
                              struct sourcepos *);
static void visit_slice_node(struct emit *, struct slice_node *,
                             struct sourcepos *);
static void visit_string_node(struct emit *, struct string_node *,
                              struct sourcepos *);
static void visit_subscript_node(struct emit *, struct subscript_node *,
                                 struct sourcepos *);
static void visit_super_node(struct emit *, struct super_node *,
                             struct sourcepos *);
static void visit_switch_node(struct emit *, struct switch_node *,
                              struct sourcepos *);
static void visit_ternary_node(struct emit *, struct ternary_node *,
                               struct sourcepos *);
static void visit_try_catch_node(struct emit *, struct try_catch_node *,
                                 struct sourcepos *);
static void visit_unary_op_node(struct emit *, struct unary_op_node *,
                                struct sourcepos *);
static void visit_while_node(struct emit *, struct while_node *,
                             struct sourcepos *);

static vm_inst_t vm_inst_new(uint16_t opcode, uint16_t opshort, uint32_t op);
static vm_inst_t bin_op_inst_new(bin_op_type_t);
static vm_inst_t build_array_inst_new(int);
static vm_inst_t build_class_inst_new(struct emit *, struct code_obj *, bool);
static vm_inst_t build_func_inst_new(struct emit *, struct code_obj *,
                                     struct vec *, bool);
static vm_inst_t build_handler_inst_new(struct emit *, struct code_obj *);
static vm_inst_t build_obj_inst_new(struct emit *, struct vec *);
static vm_inst_t destructure_array_inst_new(struct emit *, int);
static vm_inst_t destructure_object_inst_new(struct emit *, char *);
static vm_inst_t import_inst_new(struct emit *, struct vec *,
                                 struct code_obj *);
static vm_inst_t invoke_inst_new(int, bool);
static vm_inst_t iter_next_inst_new(struct emit *, char *);
static vm_inst_t jump_inst_new(int);
static vm_inst_t jump_if_false_inst_new(int);
static vm_inst_t jump_if_full_inst_new(int);
static vm_inst_t load_attrib_inst_new(struct emit *, char *, bool);
static vm_inst_t load_fast_inst_new(int);
static vm_inst_t load_id_inst_new(struct emit *, char *);
static vm_inst_t load_const_inst_new(int);
static vm_inst_t store_attrib_inst_new(struct emit *, char *);
static vm_inst_t store_fast_inst_new(int, bool);
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
static void add_inst(struct emit *, vm_inst_t, struct sourcepos *);
static int new_label();
static void place_label(struct emit *, int);

static void enter_scope(struct emit *);
static void leave_scope(struct emit *);
static int get_symbol(struct emit *, char *);
static bool has_symbol(struct emit *, char *);
static int handle_symbol(struct emit *, char *);

struct code_obj *compile_ast(struct ast_node *ast,
                             struct sourcefile *sourcefile, struct vm *vm) {
  struct emit emit;
  emit.code_obj = code_obj_new(clone_str("__module__"), true);
  emit.code_obj->sourcefile = sourcefile;
  emit.ret_type = NULL;
  emit.symtable = vec_new();
  emit.func = NULL;
  emit.class = NULL;
  emit.vm = vm;

  visit_code_block_node(&emit, ast->inner, true);
  add_inst(&emit, vm_inst_new(INST_LOAD_NONE, 0, 0), ast->sourcepos);
  add_inst(&emit, vm_inst_new(INST_RETURN, 0, 0), ast->sourcepos);

  vec_free(emit.symtable);
  return emit.code_obj;
}

static void visit_ast_node(struct emit *emit, struct ast_node *node) {
  switch (node->type) {
    case ARRAY_DECL_NODE:
      visit_array_decl_node(emit, node->inner, node->sourcepos);
      break;
    case ATTRIB_NODE:
      visit_attrib_node(emit, node->inner, node->sourcepos);
      break;
    case BIN_OP_NODE:
      visit_bin_op_node(emit, node->inner, node->sourcepos);
      break;
    case BREAK_NODE:
      visit_break_node(emit, node->sourcepos);
      break;
    case CLASS_DECL_NODE:
      visit_class_decl_node(emit, node->inner, node->sourcepos);
      break;
    case CODE_BLOCK_NODE:
      visit_code_block_node(emit, node->inner, true);
      break;
    case CONTINUE_NODE:
      visit_continue_node(emit, node->sourcepos);
      break;
    case DELETE_NODE:
      visit_delete_node(emit, node->inner, node->sourcepos);
      break;
    case DO_WHILE_NODE:
      visit_do_while_node(emit, node->inner, node->sourcepos);
      break;
    case EXPR_STMT_NODE:
      visit_expr_stmt_node(emit, node->inner, node->sourcepos);
      break;
    case FOR_NODE:
      visit_for_node(emit, node->inner, node->sourcepos);
      break;
    case FOREACH_NODE:
      visit_foreach_node(emit, node->inner, node->sourcepos);
      break;
    case FUNC_DECL_NODE:
      visit_func_decl_node(emit, node->inner, node->sourcepos);
      break;
    case ID_NODE:
      visit_id_node(emit, node->inner, node->sourcepos);
      break;
    case IF_NODE:
      visit_if_node(emit, node->inner, node->sourcepos);
      break;
    case IMPORT_NODE:
      visit_import_node(emit, node->inner, node->sourcepos);
      break;
    case INVOKE_NODE:
      visit_invoke_node(emit, node->inner, node->sourcepos);
      break;
    case NUM_NODE:
      visit_num_node(emit, node->inner, node->sourcepos);
      break;
    case OBJ_DECL_NODE:
      visit_obj_decl_node(emit, node->inner, node->sourcepos);
      break;
    case RAISE_NODE:
      visit_raise_node(emit, node->inner, node->sourcepos);
      break;
    case RETURN_NODE:
      visit_return_node(emit, node->inner, node->sourcepos);
      break;
    case SLICE_NODE:
      visit_slice_node(emit, node->inner, node->sourcepos);
      break;
    case STRING_NODE:
      visit_string_node(emit, node->inner, node->sourcepos);
      break;
    case SUBSCRIPT_NODE:
      visit_subscript_node(emit, node->inner, node->sourcepos);
      break;
    case SUPER_NODE:
      visit_super_node(emit, node->inner, node->sourcepos);
      break;
    case SWITCH_NODE:
      visit_switch_node(emit, node->inner, node->sourcepos);
      break;
    case TERNARY_NODE:
      visit_ternary_node(emit, node->inner, node->sourcepos);
      break;
    case TRY_CATCH_NODE:
      visit_try_catch_node(emit, node->inner, node->sourcepos);
      break;
    case UNARY_OP_NODE:
      visit_unary_op_node(emit, node->inner, node->sourcepos);
      break;
    case WHILE_NODE:
      visit_while_node(emit, node->inner, node->sourcepos);
      break;
  }
}

static void visit_array_decl_node(struct emit *emit,
                                  struct array_decl_node *node,
                                  struct sourcepos *sourcepos) {
  for (int i = 0; i < node->values->len; ++i) {
    visit_ast_node(emit, vec_get(node->values, i));
  }
  add_inst(emit, build_array_inst_new(node->values->len), sourcepos);
}

static void visit_attrib_node(struct emit *emit, struct attrib_node *node,
                              struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->target);
  add_inst(emit,
           load_attrib_inst_new(emit, clone_str(node->attrib),
                                node->null_coalescing),
           sourcepos);
}

static void visit_bin_op_node(struct emit *emit, struct bin_op_node *node,
                              struct sourcepos *sourcepos) {
  if (node->type == BIN_OP_ASSIGN || node->type == BIN_OP_ASSIGN_SHORT) {
    visit_ast_node(emit, node->right);
    switch (node->left->type) {
      case ARRAY_DECL_NODE: {
        struct array_decl_node *array_node = node->left->inner;
        for (int i = 0; i < array_node->values->len; ++i) {
          struct ast_node *item = vec_get(array_node->values, i);

          if (item->type != ID_NODE) {
            struct strbuf *strbuf = strbuf_new();
            strbuf_append_str(strbuf, "Invalid use of ");
            strbuf_append_str(strbuf, ast_type_t_names[item->type]);
            strbuf_append_str(strbuf, " in array destructure!");
            char *message = strbuf_done(strbuf);
            vm_raise(emit->vm, obj_compile_error_new(message, item->sourcepos));
            free(message);
            return;
          }

          struct id_node *id_node = item->inner;
          add_inst(emit, destructure_array_inst_new(emit, i), item->sourcepos);
          if (id_node->type != NULL) {
            visit_ast_node(emit, id_node->type);
          }
          add_inst(emit,
                   store_fast_inst_new(handle_symbol(emit, id_node->id),
                                       id_node->type != NULL),
                   sourcepos);
          add_inst(emit, vm_inst_new(INST_POP, 0, 0), item->sourcepos);
        }
      } break;
      case ATTRIB_NODE: {
        struct attrib_node *attrib_node = node->left->inner;
        visit_ast_node(emit, attrib_node->target);
        add_inst(emit,
                 store_attrib_inst_new(emit, clone_str(attrib_node->attrib)),
                 sourcepos);
      } break;
      case ID_NODE: {
        struct id_node *id_node = node->left->inner;
        if (id_node->type != NULL) {
          visit_ast_node(emit, id_node->type);
        }
        add_inst(emit,
                 store_fast_inst_new(handle_symbol(emit, id_node->id),
                                     id_node->type != NULL),
                 sourcepos);
      } break;
      case OBJ_DECL_NODE: {
        struct obj_decl_node *obj_node = node->left->inner;
        for (int i = 0; i < obj_node->keys->len; ++i) {
          char *key = vec_get(obj_node->keys, i);
          struct ast_node *val_node = vec_get(obj_node->values, i);
          if (val_node->type != ID_NODE) {
            struct strbuf *strbuf = strbuf_new();
            strbuf_append_str(strbuf, "Invalid use of ");
            strbuf_append_str(strbuf, ast_type_t_names[val_node->type]);
            strbuf_append_str(strbuf, " in object destructure!");
            char *message = strbuf_done(strbuf);
            vm_raise(emit->vm,
                     obj_compile_error_new(message, val_node->sourcepos));
            free(message);
            return;
          }

          struct id_node *val_id = val_node->inner;
          add_inst(emit, destructure_object_inst_new(emit, key),
                   val_node->sourcepos);
          add_inst(emit,
                   store_fast_inst_new(handle_symbol(emit, val_id->id),
                                       val_id->type != NULL),
                   sourcepos);
          add_inst(emit, vm_inst_new(INST_POP, 0, 0), val_node->sourcepos);
        }
      } break;
      case SUBSCRIPT_NODE: {
        struct subscript_node *subscript_node = node->left->inner;
        visit_ast_node(emit, subscript_node->key);
        visit_ast_node(emit, subscript_node->target);
        add_inst(emit, vm_inst_new(INST_STORE_SUBSCRIPT, 0, 0), sourcepos);
      } break;
      default: {
        struct strbuf *strbuf = strbuf_new();
        strbuf_append_str(strbuf, "Cannot assign to ");
        strbuf_append_str(strbuf, ast_type_t_names[node->left->type]);
        strbuf_append(strbuf, '!');
        char *message = strbuf_done(strbuf);
        vm_raise(emit->vm,
                 obj_compile_error_new(message, node->left->sourcepos));
        free(message);
        return;
      }
    }
  } else {
    visit_ast_node(emit, node->left);
    visit_ast_node(emit, node->right);
    add_inst(emit, bin_op_inst_new(node->type), sourcepos);
  }
}

static void visit_break_node(struct emit *emit, struct sourcepos *sourcepos) {
  if (emit->code_obj->break_labels->len <= 0) {
    vm_raise(emit->vm,
             obj_compile_error_new("Incorrect use of break.", sourcepos));
  }

  add_inst(emit,
           jump_inst_new((uintptr_t)vec_pop(emit->code_obj->break_labels)),
           sourcepos);
}

static void visit_class_decl_node(struct emit *emit,
                                  struct class_decl_node *node,
                                  struct sourcepos *sourcepos) {
  if (node->extends != NULL) {
    visit_ast_node(emit, node->extends);
  }

  struct code_obj *class = code_obj_new(clone_str(node->name), false);

  int symbol_idx_swp = next_sym_idx;
  if (emit->class == NULL) {
    next_sym_idx = 0;
    emit->class = class;
  }

  struct code_obj *swp = emit->code_obj;
  emit->code_obj = class;
  visit_ast_node(emit, node->body);
  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0), sourcepos);
  emit->code_obj = swp;

  if (emit->class == class) {
    emit->class = NULL;
    next_sym_idx = symbol_idx_swp;
  }

  add_inst(emit, build_class_inst_new(emit, class, node->extends != NULL),
           sourcepos);
}

static void visit_code_block_node(struct emit *emit,
                                  struct code_block_node *node,
                                  bool new_scope) {
  if (new_scope) {
    enter_scope(emit);
  }
  for (int i = 0; i < node->children->len; ++i) {
    visit_ast_node(emit, vec_get(node->children, i));
  }
  if (new_scope) {
    leave_scope(emit);
  }
}

static void visit_continue_node(struct emit *emit,
                                struct sourcepos *sourcepos) {
  if (emit->code_obj->cont_labels->len <= 0) {
    vm_raise(emit->vm,
             obj_compile_error_new("Incorrect use of continue.", sourcepos));
  }
  add_inst(emit, jump_inst_new((uintptr_t)vec_pop(emit->code_obj->cont_labels)),
           sourcepos);
}

static void visit_delete_node(struct emit *emit, struct delete_node *node,
                              struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_DELETE, 0, 0), sourcepos);
}

static void visit_do_while_node(struct emit *emit, struct while_node *node,
                                struct sourcepos *sourcepos) {
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
  add_inst(emit, jump_if_false_inst_new(end), sourcepos);
  add_inst(emit, jump_inst_new(body), sourcepos);

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void visit_expr_stmt_node(struct emit *emit, struct expr_stmt_node *node,
                                 struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->expr);
  add_inst(emit, vm_inst_new(INST_POP, 0, 0), sourcepos);
}

static void visit_for_node(struct emit *emit, struct for_node *node,
                           struct sourcepos *sourcepos) {
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
  add_inst(emit, jump_if_false_inst_new(end), sourcepos);
  visit_ast_node(emit, node->body);
  visit_ast_node(emit, node->repeated);
  add_inst(emit, jump_inst_new(body), sourcepos);

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void visit_foreach_node(struct emit *emit, struct foreach_node *node,
                               struct sourcepos *sourcepos) {
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
  add_inst(emit, vm_inst_new(INST_ITER, 0, 0), sourcepos);
  add_inst(emit, store_fast_inst_new(handle_symbol(emit, id), false),
           sourcepos);
  add_inst(emit, vm_inst_new(INST_POP, 0, 0), sourcepos);

  place_label(emit, body);
  add_inst(emit, load_fast_inst_new(get_symbol(emit, id)), sourcepos);
  add_inst(emit, jump_if_full_inst_new(end),
           sourcepos);  // note to decrement iter if full
  add_inst(emit, store_fast_inst_new(handle_symbol(emit, node->id), false),
           sourcepos);
  add_inst(emit, vm_inst_new(INST_POP, 0, 0), sourcepos);
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body), sourcepos);

  place_label(emit, end);
  leave_scope(emit);

  free(id);
}

static void visit_func_decl_node(struct emit *emit, struct func_decl_node *node,
                                 struct sourcepos *sourcepos) {
  struct code_obj *func = code_obj_new(clone_str(node->name), false);
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
  for (int i = 0; i < node->params->len; ++i) {
    struct ast_node *id_ast = vec_get(node->params, i);
    struct id_node *id_node = id_ast->inner;
    int sym = handle_symbol(emit, id_node->id);
    vec_push(func_params, (void *)(uintptr_t)sym);
    if (id_node->type != NULL) {
      visit_ast_node(emit, id_node->type);
      add_inst(emit, vm_inst_new(INST_TYPECHECK_FAST, 0, (uintptr_t)sym),
               id_ast->sourcepos);
    }
  }

  if (node->body->type == CODE_BLOCK_NODE) {
    visit_code_block_node(emit, node->body->inner, false);
  } else {
    visit_ast_node(emit, node->body);
  }
  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0), sourcepos);

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

  add_inst(emit, build_func_inst_new(emit, func, func_params, closure),
           sourcepos);

  if (node->name != NULL) {
    if (emit->symtable->len > 1) {
      add_inst(emit,
               store_fast_inst_new(handle_symbol(emit, node->name), false),
               sourcepos);
    } else {
      add_inst(emit, store_id_inst_new(emit, clone_str(node->name)), sourcepos);
    }
    add_inst(emit, vm_inst_new(INST_POP, 0, 0), sourcepos);
  }
}

static void visit_id_node(struct emit *emit, struct id_node *node,
                          struct sourcepos *sourcepos) {
  if (strcmp(node->id, "true") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_TRUE, 0, 0), sourcepos);
  } else if (strcmp(node->id, "false") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_FALSE, 0, 0), sourcepos);
  } else if (strcmp(node->id, "none") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  } else if (strcmp(node->id, "self") == 0) {
    add_inst(emit, vm_inst_new(INST_LOAD_SELF, 0, 0), sourcepos);
  } else if (get_symbol(emit, node->id) != -1) {
    add_inst(emit, load_fast_inst_new(get_symbol(emit, node->id)), sourcepos);
  } else {
    add_inst(emit, load_id_inst_new(emit, clone_str(node->id)), sourcepos);
  }
}

static void visit_if_node(struct emit *emit, struct if_node *node,
                          struct sourcepos *sourcepos) {
  int else_ = new_label();
  int end = new_label();
  visit_ast_node(emit, node->predicate);
  add_inst(emit, jump_if_false_inst_new(else_), sourcepos);
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(end), sourcepos);
  place_label(emit, else_);
  if (node->else_body != NULL) {
    visit_ast_node(emit, node->else_body);
  }
  place_label(emit, end);
}

static void visit_import_node(struct emit *emit, struct import_node *node,
                              struct sourcepos *sourcepos) {
  struct code_obj *mod =
      compile_module_for_import(emit->code_obj, node->from, emit->vm);

  add_inst(emit, import_inst_new(emit, node->imports, mod), sourcepos);
}

static void visit_invoke_node(struct emit *emit, struct invoke_node *node,
                              struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->target);
  for (int i = 0; i < node->args->len; ++i)
    visit_ast_node(emit, vec_get(node->args, i));
  add_inst(emit, invoke_inst_new(node->args->len, node->null_coalescing),
           sourcepos);
}

static void visit_num_node(struct emit *emit, struct num_node *node,
                           struct sourcepos *sourcepos) {
  int idx = -1;
  for (int i = 0; i < emit->code_obj->consts->len; ++i) {
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
  add_inst(emit, load_const_inst_new(idx), sourcepos);
}

static void visit_obj_decl_node(struct emit *emit, struct obj_decl_node *node,
                                struct sourcepos *sourcepos) {
  for (int i = 0; i < node->values->len; ++i) {
    visit_ast_node(emit, vec_get(node->values, i));
  }
  add_inst(emit, build_obj_inst_new(emit, string_vec_clone(node->keys)),
           sourcepos);
}

static void visit_raise_node(struct emit *emit, struct raise_node *node,
                             struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->value);
  add_inst(emit, vm_inst_new(INST_RAISE, 0, 0), sourcepos);
}

static void visit_return_node(struct emit *emit, struct return_node *node,
                              struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->value);
  if (emit->ret_type != NULL) {
    visit_ast_node(emit, emit->ret_type);
    add_inst(emit, vm_inst_new(INST_TYPECHECK, 0, 0), sourcepos);
  }
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0), sourcepos);
}

static void visit_slice_node(struct emit *emit, struct slice_node *node,
                             struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->target);
  if (node->start != NULL) {
    visit_ast_node(emit, node->start);
  } else {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  }
  if (node->end != NULL) {
    visit_ast_node(emit, node->end);
  } else {
    add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  }
  add_inst(emit, vm_inst_new(INST_SLICE, node->null_coalescing, 0), sourcepos);
}

static void visit_string_node(struct emit *emit, struct string_node *node,
                              struct sourcepos *sourcepos) {
  int idx = -1;
  for (int i = 0; i < emit->code_obj->consts->len; ++i) {
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
  add_inst(emit, load_const_inst_new(idx), sourcepos);
}

static void visit_subscript_node(struct emit *emit, struct subscript_node *node,
                                 struct sourcepos *sourcepos) {
  visit_ast_node(emit, node->key);
  visit_ast_node(emit, node->target);
  add_inst(emit, vm_inst_new(INST_LOAD_SUBSCRIPT, node->null_coalescing, 0),
           sourcepos);
}

static void visit_super_node(struct emit *emit, struct super_node *node,
                             struct sourcepos *sourcepos) {
  for (int i = 0; i < node->args->len; ++i) {
    visit_ast_node(emit, vec_get(node->args, i));
  }
  add_inst(emit, super_inst_new(node->args->len), sourcepos);
}

static void visit_switch_node(struct emit *emit, struct switch_node *node,
                              struct sourcepos *sourcepos) {
  char *id = tmp_symbol();
  int default_case = new_label();
  int switch_end = new_label();

  visit_ast_node(emit, node->target);
  add_inst(emit, store_fast_inst_new(handle_symbol(emit, id), false),
           node->target->sourcepos);
  add_inst(emit, vm_inst_new(INST_POP, 0, 0), node->target->sourcepos);

  int case_labels[node->cases->len];
  int case_body_labels[node->cases->len];
  for (int i = 0; i < node->cases->len; ++i) {
    case_labels[i] = new_label();
    if (vec_get(node->case_bodies, i) != NULL) {
      case_body_labels[i] = new_label();
    } else {
      case_body_labels[i] = -1;
    }
  }

  for (int i = 0; i < node->cases->len; ++i) {
    struct ast_node *case_ast = vec_get(node->cases, i);
    struct ast_node *case_body = vec_get(node->case_bodies, i);

    place_label(emit, case_labels[i]);
    add_inst(emit, load_fast_inst_new(handle_symbol(emit, id)),
             case_ast->sourcepos);

    if (case_ast->type == UNARY_OP_NODE &&
        ((struct unary_op_node *)case_ast->inner)->for_switch) {
      add_inst(emit, unary_op_inst_new(UNARY_OP_NOT), case_ast->sourcepos);
    } else if (case_ast->type == BIN_OP_NODE &&
               ((struct bin_op_node *)case_ast->inner)->for_switch) {
      struct bin_op_node *bin_op_ctx = case_ast->inner;
      visit_ast_node(emit, bin_op_ctx->right);
      add_inst(emit, bin_op_inst_new(bin_op_ctx->type), case_ast->sourcepos);
    } else {
      visit_ast_node(emit, case_ast);
      add_inst(emit, bin_op_inst_new(BIN_OP_EQ), case_ast->sourcepos);
    }

    if (i == node->cases->len - 1 && node->default_case != NULL) {
      add_inst(emit, jump_if_false_inst_new(default_case), case_ast->sourcepos);
    } else if (i == node->cases->len - 1 && node->default_case == NULL) {
      add_inst(emit, jump_if_false_inst_new(switch_end), case_ast->sourcepos);
    } else {
      add_inst(emit, jump_if_false_inst_new(case_labels[i + 1]),
               case_ast->sourcepos);
    }

    int body_idx = i;
    while (case_body_labels[body_idx] == -1 && body_idx < node->cases->len) {
      body_idx++;
    }
    add_inst(emit, jump_inst_new(case_body_labels[body_idx]),
             case_ast->sourcepos);

    struct labels_ctx labels = {
        .break_count = emit->code_obj->break_labels->len,
        .cont_count = emit->code_obj->cont_labels->len,
    };
    vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)switch_end);

    if (case_body != NULL) {
      place_label(emit, case_body_labels[i]);
      visit_ast_node(emit, case_body);
    }

    restore_labels(emit, labels);
  }

  if (node->default_case != NULL) {
    place_label(emit, default_case);

    struct labels_ctx labels = {
        .break_count = emit->code_obj->break_labels->len,
        .cont_count = emit->code_obj->cont_labels->len,
    };
    vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)switch_end);

    visit_ast_node(emit, node->default_case);
    restore_labels(emit, labels);
  }

  place_label(emit, switch_end);

  free(id);
}

static void visit_ternary_node(struct emit *emit, struct ternary_node *node,
                               struct sourcepos *sourcepos) {
  int false_label = new_label();
  int end_label = new_label();

  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(false_label), sourcepos);
  visit_ast_node(emit, node->true_value);
  add_inst(emit, jump_inst_new(end_label), sourcepos);

  place_label(emit, false_label);
  visit_ast_node(emit, node->false_value);

  place_label(emit, end_label);
}

static void visit_try_catch_node(struct emit *emit, struct try_catch_node *node,
                                 struct sourcepos *sourcepos) {
  struct code_obj *handler = code_obj_new(NULL, false);
  struct code_obj *swp = emit->code_obj;
  emit->code_obj = handler;

  enter_scope(emit);

  handler->params = vec_new();

  if (node->id != NULL) {
    int sym = handle_symbol(emit, node->id);
    vec_push(handler->params, (void *)(uintptr_t)sym);
  }

  if (node->catch->type == CODE_BLOCK_NODE) {
    visit_code_block_node(emit, node->catch->inner, false);
  } else {
    visit_ast_node(emit, node->catch);
  }

  add_inst(emit, vm_inst_new(INST_LOAD_NONE, 0, 0), sourcepos);
  add_inst(emit, vm_inst_new(INST_RETURN, 0, 0), sourcepos);

  leave_scope(emit);
  emit->code_obj = swp;
  handler->parent = emit->code_obj;
  emit->code_obj->locals += handler->locals;

  int end = new_label();
  handler->caught_label = end;
  add_inst(emit, build_handler_inst_new(emit, handler), sourcepos);

  enter_scope(emit);
  visit_ast_node(emit, node->try);
  leave_scope(emit);

  add_inst(emit, vm_inst_new(INST_POP_HANDLER, 0, 0), sourcepos);
  place_label(emit, end);
}

static void visit_unary_op_node(struct emit *emit, struct unary_op_node *node,
                                struct sourcepos *sourcepos) {
  if (node->type == UNARY_OP_POST_DEC || node->type == UNARY_OP_POST_INC) {
    struct bin_op_node *bin_op = node->target->inner;
    visit_ast_node(emit, bin_op->left);
    visit_ast_node(emit, node->target);
  } else {
    visit_ast_node(emit, node->target);
  }

  add_inst(emit, unary_op_inst_new(node->type), sourcepos);
}

static void visit_while_node(struct emit *emit, struct while_node *node,
                             struct sourcepos *sourcepos) {
  int body = new_label();
  int inner_body = new_label();
  int else_ = new_label();
  int end = new_label();

  struct labels_ctx labels = {
      .break_count = emit->code_obj->break_labels->len,
      .cont_count = emit->code_obj->cont_labels->len,
  };
  vec_push(emit->code_obj->break_labels, (void *)(uintptr_t)end);
  vec_push(emit->code_obj->cont_labels, (void *)(uintptr_t)body);

  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(node->else_body == NULL ? end : else_),
           sourcepos);
  add_inst(emit, jump_inst_new(inner_body), sourcepos);

  place_label(emit, body);
  visit_ast_node(emit, node->condition);
  add_inst(emit, jump_if_false_inst_new(end), sourcepos);
  place_label(emit, inner_body);
  visit_ast_node(emit, node->body);
  add_inst(emit, jump_inst_new(body), sourcepos);

  if (node->else_body != NULL) {
    place_label(emit, else_);
    visit_ast_node(emit, node->else_body);
  }

  place_label(emit, end);
  restore_labels(emit, labels);
}

static void restore_labels(struct emit *emit, struct labels_ctx labels_ctx) {
  for (int i = emit->code_obj->break_labels->len; i < labels_ctx.break_count;
       ++i) {
    vec_pop(emit->code_obj->break_labels);
  }
  for (int i = emit->code_obj->cont_labels->len; i < labels_ctx.cont_count;
       ++i) {
    vec_pop(emit->code_obj->cont_labels);
  }
}

static int tmp_sym_idx = 0;
static char *tmp_symbol() {
  char *sym = (char *)calloc(8, sizeof(char));
  sprintf(sym, "%d", tmp_sym_idx++);
  return sym;
}

static void add_inst(struct emit *emit, vm_inst_t inst,
                     struct sourcepos *sourcepos) {
  vec_push(emit->code_obj->instructions, (void *)inst);
  vec_push(emit->code_obj->sourceposes, sourcepos_inc_ref(sourcepos));
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

static vm_inst_t destructure_array_inst_new(struct emit *emit, int idx) {
  vec_push(emit->code_obj->consts, obj_inc_ref(obj_num_new(false, idx, 0)));
  return vm_inst_new(INST_DESTRUCTURE_ARRAY, 0,
                     emit->code_obj->consts->len - 1);
}

static vm_inst_t destructure_object_inst_new(struct emit *emit, char *key) {
  vec_push(emit->code_obj->consts, obj_inc_ref(obj_string_new(key)));
  return vm_inst_new(INST_DESTRUCTURE_OBJECT, 0,
                     emit->code_obj->consts->len - 1);
}

static vm_inst_t import_inst_new(struct emit *emit, struct vec *imports,
                                 struct code_obj *mod) {
  vec_push(emit->code_obj->vecs, imports);
  vec_push(emit->code_obj->code_objs, mod);

  return vm_inst_new(INST_IMPORT, (uint16_t)emit->code_obj->vecs->len - 1,
                     emit->code_obj->code_objs->len - 1);
}

static vm_inst_t invoke_inst_new(int arg_count, bool null_coalescing) {
  return vm_inst_new(INST_INVOKE, null_coalescing, arg_count);
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

static vm_inst_t load_attrib_inst_new(struct emit *emit, char *attrib,
                                      bool null_coalescing) {
  vec_push(emit->code_obj->strs, attrib);
  return vm_inst_new(INST_LOAD_ATTRIB, null_coalescing,
                     emit->code_obj->strs->len - 1);
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

static vm_inst_t store_fast_inst_new(int idx, bool enforced) {
  return vm_inst_new(INST_STORE_FAST, enforced, idx);
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
