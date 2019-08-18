#ifndef _EMIT_H_
#define _EMIT_H_

#include <bin_op_type.h>
#include <emit/inst.h>
#include <parser/ast.h>
#include <parser/node_type.h>
#include <unary_op_type.h>

struct emit_state {

};

struct emit_state * emit_init ();
void                emit_free (struct emit_state * state);

void accept (struct emit_state * state, struct ast_node * node);

#endif
