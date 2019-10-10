#ifndef _INST_H_
#define _INST_H_

#include <stdlib.h>
#include <util/vector.h>

typedef enum {
    BIN_OP_INST,                 BUILD_CLOSURE_INST,         BUILD_EXCEPTION_HANDLER_INST, CALL_INST,  // 0-3
    COMPILE_MODULE_INST,         IMPORT_INST,                ITER_INST,                    ITER_NEXT_INST,       ITER_FULL_INST,   ITER_FREE_INST, // 4-9
    JUMP_INST,                   JUMP_IF_TRUE_INST,          JUMP_IF_FALSE_INST,           LIST_DECL_INST,       LOAD_ATTRIB_INST,          // 10-14
    LOAD_GLOBAL_INST,            LOAD_ID_INST,               LOAD_LOCAL_INST,              LOAD_NUMBER_INST,     LOAD_STRING_INST, LOAD_SUBSCRIPT_INST, OBJ_DECL_INST,        // 15-21
    OBJ_DESTRUCTURE_GLOBAL_INST, OBJ_DESTRUCTURE_LOCAL_INST, POP_INST, // 22-24
    POP_EXCEPTION_HANDLER_INST,  PUSH_INST,                  RAISE_INST,                   RETURN_INST,          SELF_REFERENCE_INST,         // 25-29
    STORE_ATTRIB_INST,           STORE_GLOBAL_INST,          STORE_LOCAL_INST,             STORE_SUBSCRIPT_INST,             // 30-33
    SUPER_INST,                  TYPEOF_INST,                UNARY_OP_INST,                USE_GLOBAL_INST,      USE_LOCAL_INST,  // 34-36
} inst_type_t;

struct inst {
    inst_type_t type;
    int         arg1;
    int         arg2;
    float       arg3;
    char  *     arg4;
    void  *     arg5;
    void  *     arg6;
};

#include <runtime/has_obj.h>

struct inst * inst_init (inst_type_t type);
void          free_inst (struct inst * inst);

#endif
