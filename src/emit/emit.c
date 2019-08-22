#include <emit/emit.h>

struct emit_state * emit_init () {
    struct emit_state * state;

    state                  = (struct emit_state *)calloc (1, sizeof (struct emit_state));
    state->symbol_table    = symbol_table_init ();
    state->emit_stack      = vector_init ();
    state->break_labels    = int_vector_init ();
    state->cont_labels     = int_vector_init ();
    state->next_label      = 0;

    vector_push (state->emit_stack, has_obj_init (NULL, NULL));

    return state;
}

void emit_free (struct emit_state * state) {
    symbol_table_free (state->symbol_table);
    vector_free       (state->emit_stack);
    int_vector_free   (state->break_labels);
    int_vector_free   (state->cont_labels);

    free              (state);
}

struct has_obj * emit_get_module (struct emit_state * state) {
    return vector_pop (state->emit_stack);
}

static void accept_assign        (struct emit_state * emit, struct assign_state        * state);
static void accept_attrib_access (struct emit_state * emit, struct attrib_access_state * state);
static void accept_bin_op        (struct emit_state * emit, struct bin_op_state        * state);
static void accept_block         (struct emit_state * emit, struct block_state         * state);
static void accept_break         (struct emit_state * emit);
static void accept_class         (struct emit_state * emit, struct class_state         * state);
static void accept_closure       (struct emit_state * emit, struct closure_state       * state);
static void accept_continue      (struct emit_state * emit);
static void accept_expr_stmt     (struct emit_state * emit, struct expr_stmt_state     * state);
static void accept_for           (struct emit_state * emit, struct for_state           * state);
static void accept_foreach       (struct emit_state * emit, struct foreach_state       * state);
static void accept_func_call     (struct emit_state * emit, struct func_call_state     * state);
static void accept_func_decl     (struct emit_state * emit, struct func_decl_state     * state);
static void accept_id            (struct emit_state * emit, struct id_state            * state);
static void accept_if            (struct emit_state * emit, struct if_state            * state);
static void accept_import        (struct emit_state * emit, struct import_state        * state);
static void accept_list_decl     (struct emit_state * emit, struct list_decl_state     * state);
static void accept_number        (struct emit_state * emit, struct number_state        * state);
static void accept_obj_decl      (struct emit_state * emit, struct obj_decl_state      * state);
static void accept_raise         (struct emit_state * emit, struct raise_state         * state);
static void accept_return        (struct emit_state * emit, struct return_state        * state);
static void accept_string        (struct emit_state * emit, struct string_state        * state);
static void accept_subscript     (struct emit_state * emit, struct subscript_state     * state);
static void accept_super         (struct emit_state * emit, struct super_state         * state);
static void accept_try_catch     (struct emit_state * emit, struct try_catch_state     * state);
static void accept_typeof        (struct emit_state * emit, struct typeof_state        * state);
static void accept_unary_op      (struct emit_state * emit, struct unary_op_state      * state);
static void accept_use           (struct emit_state * emit, struct use_state           * state);
static void accept_while         (struct emit_state * emit, struct while_state         * state);

static        void      emit_inst      (struct emit_state * emit, struct inst * inst);
static        void      emit_label     (struct emit_state * emit, int label);
static struct has_obj * emit_peek      (struct emit_state * emit);
static struct has_obj * emit_pop       (struct emit_state * emit);
static        void      emit_push      (struct emit_state * emit, struct has_obj * obj);
static        void      restore_labels (struct emit_state * emit, int break_count, int continue_count);
static        int       next_label     (struct emit_state * emit);

void accept (struct emit_state * state, struct ast_node * node) {
    switch (node->type) {
        case assign_node:
            accept_assign (state, (struct assign_state *)node->state);
            break;
        case attrib_access_node:
            accept_attrib_access (state, (struct attrib_access_state *)node->state);
            break;
        case bin_op_node:
            accept_bin_op (state, (struct bin_op_state *)node->state);
            break;
        case block_node:
            accept_block (state, (struct block_state *)node->state);
            break;
        case break_node:
            accept_break (state);
            break;
        case class_node:
            accept_class (state, (struct class_state *)node->state);
            break;
        case closure_node:
            accept_closure (state, (struct closure_state *)node->state);
            break;
        case continue_node:
            accept_continue (state);
            break;
        case expr_stmt_node:
            accept_expr_stmt (state, (struct expr_stmt_state *)node->state);
            break;
        case for_node:
            accept_for (state, (struct for_state *)node->state);
            break;
        case foreach_node:
            accept_foreach (state, (struct foreach_state *)node->state);
            break;
        case func_call_node:
            accept_func_call (state, (struct func_call_state *)node->state);
            break;
        case func_decl_node:
            accept_func_decl (state, (struct func_decl_state *)node->state);
            break;
        case id_node:
            accept_id (state, (struct id_state *)node->state);
            break;
        case if_node:
            accept_if (state, (struct if_state *)node->state);
            break;
        case import_node:
            accept_import (state, (struct import_state *)node->state);
            break;
        case list_decl_node:
            accept_list_decl (state, (struct list_decl_state *)node->state);
            break;
        case number_node:
            accept_number (state, (struct number_state *)node->state);
            break;
        case obj_decl_node:
            accept_obj_decl (state, (struct obj_decl_state *)node->state);
            break;
        case raise_node:
            accept_raise (state, (struct raise_state *)node->state);
            break;
        case return_node:
            accept_return (state, (struct return_state *)node->state);
            break;
        case string_node:
            accept_string (state, (struct string_state *)node->state);
            break;
        case subscript_node:
            accept_subscript (state, (struct subscript_state *)node->state);
            break;
        case super_node:
            accept_super (state, (struct super_state *)node->state);
            break;
        case try_catch_node:
            accept_try_catch (state, (struct try_catch_state *)node->state);
            break;
        case typeof_node:
            accept_typeof (state, (struct typeof_state *)node->state);
            break;
        case unary_op_node:
            accept_unary_op (state, (struct unary_op_state *)node->state);
            break;
        case use_node:
            accept_use (state, (struct use_state *)node->state);
            break;
        case while_node:
            accept_while (state, (struct while_state *)node->state);
            break;
    }
}

static void accept_assign (struct emit_state * emit, struct assign_state * state) {
    struct attrib_access_state * attrib_access_state;
    struct list_decl_state     * list_decl_state;
    struct subscript_state     * subscript_state;
    struct id_state            * id_state;
    struct ast_node            * left;
    struct vector_state        * vars;
    struct int_vector_state    * indices;
    char                       * tmp;

    accept (emit, state->right);

    left = state->left;
    switch (left->type) {
        case attrib_access_node:
            attrib_access_state = (struct attrib_access_state *)left->state;

            accept    (emit, attrib_access_state->target);
            emit_inst (emit, store_attrib_inst_init (attrib_access_state->id));
            break;
        case list_decl_node:
            list_decl_state = (struct list_decl_state *)left->state;

            vars = vector_init ();
            for (int i = 0; i < list_decl_state->elements->length; i++) {
                id_state = ((struct ast_node *)vector_get (list_decl_state->elements, i))->state;
                tmp = (char *)calloc (strlen (id_state->id) + 1, sizeof (char));
                memcpy (tmp, id_state->id, strlen (id_state->id) + 1);
                vector_push (vars, tmp);
            }

            indices = int_vector_init ();
            for (int i = 0; i < vars->length; i++) {
                int_vector_push(
                    indices,
                    handle_symbol (
                        emit->symbol_table,
                        vector_get (vars, i)
                    )
                );
            }

            if (in_global_scope (emit->symbol_table)) {
                emit_inst (emit, obj_destructure_global_inst_init (vars, indices));
            } else {
                emit_inst (emit, obj_destructure_local_inst_init (vars, indices));
            }
            break;
        case subscript_node:
            subscript_state = (struct subscript_state *)left->state;

            accept    (emit, subscript_state->index);
            accept    (emit, subscript_state->target);
            emit_inst (emit, store_subscript_inst_init ());
            break;
        case id_node:
            id_state = (struct id_state *)left->state;

            if (in_global_scope (emit->symbol_table)) {
                emit_inst (emit, store_global_inst_init (
                    handle_symbol (emit->symbol_table, id_state->id)
                ));
            } else {
                emit_inst (emit, store_local_inst_init (
                    handle_symbol (emit->symbol_table, id_state->id)
                ));
            }
            break;
        default:
            break;
    }
}

static void accept_attrib_access (struct emit_state * emit, struct attrib_access_state * state) {
    accept    (emit, state->target);
    emit_inst (emit, load_attrib_inst_init (state->id));
}

static void accept_bin_op (struct emit_state * emit, struct bin_op_state * state) {
    accept (emit, state->right);
    accept (emit, state->left);

    emit_inst (emit, bin_op_inst_init (state->type));
}

static void accept_block (struct emit_state * emit, struct block_state * state) {
    for (int i = 0; i < state->stmts->length; i++) {
        accept (emit, vector_get (state->stmts, i));
    }
}

static void accept_break (struct emit_state * emit) {
    emit_label (emit, int_vector_pop (emit->break_labels));
}

static void accept_class (struct emit_state * emit, struct class_state * state) {
    struct has_obj      * class;

    class = has_class_init (state->name, state->extends);
    has_obj_set_attrib (emit_peek (emit), state->name, class);

    emit_push (emit, class);
    accept    (emit, state->body);
    emit_pop  (emit);
}

static void accept_closure (struct emit_state * emit, struct closure_state * state) {
    struct has_obj * func;

    func = has_func_init (NULL, state->params, state->return_type);

    emit_push (emit, func);

    enter_scope (emit->symbol_table);
    accept      (emit, state->body);
    leave_scope (emit->symbol_table);

    emit_pop (emit);

    emit_inst (emit, build_closure_inst_init (func));
}

static void accept_continue (struct emit_state * emit) {
    emit_label (emit, int_vector_pop (emit->cont_labels));
}

static void accept_expr_stmt (struct emit_state * emit, struct expr_stmt_state * state) {
    accept (emit, state->expr);
    emit_inst (emit, pop_inst_init ());
}

static void accept_for (struct emit_state * emit, struct for_state * state) {
    int body_label = next_label (emit);
    int cont_label = next_label (emit);
    int end_label  = next_label (emit);

    accept     (emit, state->pre_stmt);
    emit_label (emit, body_label);
    accept     (emit, state->expr);
    emit_inst  (emit, jump_if_false_inst_init (end_label));

    int break_count = emit->break_labels   ->length;
    int cont_count  = emit->cont_labels->length;
    int_vector_push  (emit->break_labels, end_label);
    int_vector_push  (emit->cont_labels,  cont_label);

    accept (emit, state->body);
    restore_labels (emit, break_count, cont_count);

    emit_label (emit, cont_label);
    accept     (emit, state->rep_stmt);
    emit_inst  (emit, jump_inst_init (body_label));
    emit_label (emit, end_label);
}

static void accept_foreach (struct emit_state * emit, struct foreach_state * state) {
    
}

static void accept_func_call (struct emit_state * emit, struct func_call_state * state) {

}

static void accept_func_decl (struct emit_state * emit, struct func_decl_state * state) {
    struct has_obj * func;

    func = has_func_init (state->name, state->params, state->return_type);
    has_obj_set_attrib (emit_peek (emit), state->name, func);

    emit_push   (emit, func);

    enter_scope (emit->symbol_table);
    accept      (emit, state->body);
    leave_scope (emit->symbol_table);

    emit_pop    (emit);
}

static void accept_id (struct emit_state * emit, struct id_state * state) {

}

static void accept_if (struct emit_state * emit, struct if_state * state) {

}

static void accept_import (struct emit_state * emit, struct import_state * state) {

}

static void accept_list_decl (struct emit_state * emit, struct list_decl_state * state) {

}

static void accept_number (struct emit_state * emit, struct number_state * state) {

}

static void accept_obj_decl (struct emit_state * emit, struct obj_decl_state * state) {

}

static void accept_raise (struct emit_state * emit, struct raise_state * state) {

}

static void accept_return (struct emit_state * emit, struct return_state * state) {

}

static void accept_string (struct emit_state * emit, struct string_state * state) {

}

static void accept_subscript (struct emit_state * emit, struct subscript_state * state) {

}

static void accept_super (struct emit_state * emit, struct super_state * state) {

}

static void accept_try_catch (struct emit_state * emit, struct try_catch_state * state) {

}

static void accept_typeof (struct emit_state * emit, struct typeof_state * state) {

}

static void accept_unary_op (struct emit_state * emit, struct unary_op_state * state) {

}

static void accept_use (struct emit_state * emit, struct use_state * state) {

}

static void accept_while (struct emit_state * emit, struct while_state * state) {

}

static void emit_inst (struct emit_state * emit, struct inst * inst) {
    has_obj_emit (emit_peek (emit), inst);
}

static void emit_label (struct emit_state * emit, int label) {
    has_obj_emit_label (emit_peek (emit), label);
}

static struct has_obj * emit_peek (struct emit_state * emit) {
    return vector_peek (emit->emit_stack);
}

static struct has_obj * emit_pop (struct emit_state * emit) {
    return vector_pop (emit->emit_stack);
}

static void emit_push (struct emit_state * emit, struct has_obj * obj) {
    vector_push (emit->emit_stack, obj);
}

static void restore_labels (struct emit_state * emit, int break_count, int continue_count) {
    while (emit->break_labels->length > break_count) {
        int_vector_pop (emit->break_labels);
    }

    while (emit->cont_labels->length > continue_count) {
        int_vector_pop (emit->cont_labels);
    }
}

static int next_label (struct emit_state * emit) {
    return emit->next_label++;
}
