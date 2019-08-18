#include <emit/emit.h>

struct emit_state * emit_init () {

}

void emit_free (struct emit_state * state) {
    free (state);
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

}

static void accept_attrib_access (struct emit_state * emit, struct attrib_access_state * state) {

}

static void accept_bin_op (struct emit_state * emit, struct bin_op_state * state) {

}

static void accept_block (struct emit_state * emit, struct block_state * state) {

}

static void accept_break (struct emit_state * emit) {

}

static void accept_class (struct emit_state * emit, struct class_state * state) {

}

static void accept_closure (struct emit_state * emit, struct closure_state * state) {

}

static void accept_continue (struct emit_state * emit) {

}

static void accept_expr_stmt (struct emit_state * emit, struct expr_stmt_state * state) {

}

static void accept_for (struct emit_state * emit, struct for_state * state) {

}

static void accept_foreach (struct emit_state * emit, struct foreach_state * state) {

}

static void accept_func_call (struct emit_state * emit, struct func_call_state * state) {

}

static void accept_func_decl (struct emit_state * emit, struct func_decl_state * state) {

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
