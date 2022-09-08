#ifndef _VM_H_
#define _VM_H_

#include <ast.h>
#include <builtins.h>
#include <codeobj.h>
#include <ds/intmap.h>
#include <ds/vec.h>
#include <hassium.h>
#include <object.h>
#include <parser.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t vm_inst_t;

typedef enum {
  INST_BIN_OP,
  INST_BUILD_ARRAY,
  INST_BUILD_CLASS,
  INST_BUILD_FUNC,
  INST_BUILD_HANDLER,
  INST_BUILD_OBJ,
  INST_DELETE,
  INST_DESTRUCTURE_ARRAY,
  INST_DESTRUCTURE_OBJECT,
  INST_IMPORT,
  INST_INVOKE,
  INST_ITER,
  INST_ITER_NEXT,
  INST_JUMP,
  INST_JUMP_IF_FALSE,
  INST_JUMP_IF_FULL,
  INST_JUMP_IF_TRUE,
  INST_LOAD_ATTRIB,
  INST_LOAD_FALSE,
  INST_LOAD_FAST,
  INST_LOAD_ID,
  INST_LOAD_NONE,
  INST_LOAD_CONST,
  INST_LOAD_SELF,
  INST_LOAD_SUBSCRIPT,
  INST_LOAD_TRUE,
  INST_POP,
  INST_POP_HANDLER,
  INST_RAISE,
  INST_RETURN,
  INST_SELFREF,
  INST_SLICE,
  INST_STORE_ATTRIB,
  INST_STORE_ID,
  INST_STORE_FAST,
  INST_STORE_SUBSCRIPT,
  INST_SUPER,
  INST_TYPECHECK,
  INST_TYPECHECK_FAST,
  INST_UNARY_OP,
} vm_opcode_t;

struct handler_label {
  struct code_obj *code_obj;
  int label;
};

struct vm *vm_new();
void vm_free(struct vm *);

struct obj *vm_run(struct vm *, struct code_obj *, struct obj *);
void vm_raise(struct vm *, struct obj *);
void debug();

#endif
