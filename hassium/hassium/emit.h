#ifndef _EMIT_H_
#define _EMIT_H_

#include <ast.h>
#include <builtins.h>
#include <codeobj.h>
#include <ds/hashmap.h>
#include <ds/intmap.h>
#include <ds/vec.h>
#include <object.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

struct code_obj *compile_ast(struct ast_node *, struct sourcefile *sourcefile,
                             struct vm *);

#endif