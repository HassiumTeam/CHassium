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
#include <stdlib.h>

struct vm *vm_new();
void vm_free(struct vm *);

struct obj *vm_run(struct obj *, struct vm *, struct code_obj *);

typedef enum {
  INST_BIN_OP,
  INST_BUILD_ARRAY,
  INST_BUILD_CLASS,
  INST_BUILD_FUNC,
  INST_BUILD_OBJ,
  INST_IMPORT,
  INST_INVOKE,
  INST_ITER,
  INST_ITER_NEXT,
  INST_JUMP,
  INST_JUMP_IF_FALSE,
  INST_JUMP_IF_FULL,
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
  INST_RAISE,
  INST_RETURN,
  INST_SELFREF,
  INST_STORE_ATTRIB,
  INST_STORE_ID,
  INST_STORE_FAST,
  INST_STORE_SUBSCRIPT,
  INST_SUPER,
  INST_TYPECHECK,
  INST_TYPECHECK_FAST,
  INST_UNARY_OP,
} vm_inst_t;

struct vm_inst {
  vm_inst_t type;
  void *inner;
};

struct bin_op_inst {
  bin_op_type_t type;
};

struct build_array_inst {
  int count;
};

struct build_class_inst {
  struct code_obj *code_obj;
  bool extends;
};

struct build_func_inst {
  struct code_obj *code_obj;
  struct vec *params;
  bool closure;
};

struct build_obj_inst {
  struct vec *keys;
};

struct fast_inst {
  int idx;
};

struct invoke_inst {
  int arg_count;
};

struct jump_inst {
  int label;
};

struct import_inst {
  struct vec *imports;
  struct code_obj *mod;
};

struct iter_next_inst {
  char *id;
};

struct load_attrib_inst {
  char *attrib;
};

struct load_id_inst {
  char *id;
};

struct load_const_inst {
  int idx;
};

struct store_attrib_inst {
  char *attrib;
};

struct store_id_inst {
  char *id;
};

struct super_inst {
  int arg_count;
};

struct unary_op_inst {
  unary_op_type_t type;
};

#endif
