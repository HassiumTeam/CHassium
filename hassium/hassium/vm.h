#ifndef _VM_H_
#define _VM_H_

#include <ds/intdict.h>
#include <ds/vec.h>
#include <parser.h>
#include <stdbool.h>
#include <stdlib.h>

struct code_obj
{
    char *name;
    struct vec *instructions;
    struct intdict *labels;
};

struct code_obj *code_obj_new(char *);

struct vm
{
    struct vec *frames;
};

struct vm *vm_new();
void vm_free(struct vm *);

void vm_run(struct vm *, struct code_obj *);

typedef enum
{
    INST_BIN_OP,
    INST_BUILD_CLASS,
    INST_BUILD_FUNC,
    INST_IMPORT,
    INST_INVOKE,
    INST_ITER,
    INST_ITERNEXT,
    INST_JUMP,
    INST_JUMP_IF_FALSE,
    INST_JUMP_IF_FULL,
    INST_LOAD_ATTRIB,
    INST_LOAD_ID,
    INST_LOAD_NUM,
    INST_LOAD_STR,
    INST_LOAD_SUBSCRIPT,
    INST_POP,
    INST_RAISE,
    INST_RETURN,
    INST_SELFREF,
    INST_STORE_ATTRIB,
    INST_STORE_ID,
    INST_STORE_SUBSCRIPT,
    INST_SUPER,
    INST_UNARY_OP,
} vm_inst_t;

struct vm_inst
{
    vm_inst_t type;
    void *inner;
};

struct bin_op_inst
{
    bin_op_type_t type;
};

struct build_class_inst
{
    struct code_obj *code_obj;
    bool extends;
};

struct build_func_inst
{
    struct code_obj *code_obj;
    struct vec *params;
    bool with_return_type;
};

struct invoke_inst
{
    int arg_count;
};

struct jump_inst
{
    int label;
};

struct load_attrib_inst
{
    char *attrib;
};

struct load_id_inst
{
    char *id;
};

struct load_num_inst
{
    float value;
};

struct load_str_inst
{
    char *str;
};

struct store_attrib_inst
{
    char *attrib;
};

struct store_id_inst
{
    char *id;
};

struct super_inst
{
    int arg_count;
};

struct unary_op_inst
{
    unary_op_type_t type;
};

#endif