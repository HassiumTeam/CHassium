#ifndef _EMIT_H_
#define _EMIT_H_

#include <bin_op_type.h>
#include <emit/inst.h>
#include <emit/symbol_table.h>
#include <has_lib/has_class.h>
#include <has_lib/has_func.h>
#include <has_lib/has_obj.h>
#include <parser/ast.h>
#include <parser/node_type.h>
#include <stdio.h>
#include <unary_op_type.h>
#include <util/int_vector.h>
#include <util/vector.h>
#include <vm/gc.h>

struct emit_state {
    struct symbol_table_state * symbol_table;
    struct vector_state       * emit_stack;
    struct int_vector_state   * break_labels;
    struct int_vector_state   * cont_labels;
    int                         next_label;
};

struct emit_state * emit_init ();
void                emit_free (struct emit_state * state);

struct has_obj * emit_get_module (struct emit_state * state);
void             accept          (struct emit_state * state, struct ast_node * node);

#endif
