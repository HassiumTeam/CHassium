#ifndef _EMIT_H_
#define _EMIT_H_

#include <ast.h>
#include <ds/intdict.h>
#include <ds/vec.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

struct code_obj *compile_ast(struct ast_node *);

#endif