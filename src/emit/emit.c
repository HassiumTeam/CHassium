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
static void accept_try_catch     (struct emit_state * emit, struct try_catch_state     * state);
static void accept_typeof        (struct emit_state * emit, struct typeof_state        * state);
static void accept_unary_op      (struct emit_state * emit, struct unary_op_state      * state);
static void accept_use           (struct emit_state * emit, struct use_state           * state);
static void accept_while         (struct emit_state * emit, struct while_state         * state);

void accept (struct emit_state * state, struct ast_node * node) {

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
