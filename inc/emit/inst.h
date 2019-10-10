#ifndef _INST_H_
#define _INST_H_

#include <parser/ast.h>
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

struct inst * bin_op_inst_init                  (bin_op_type_t type);
struct inst * build_closure_inst_init           (void * func);
struct inst * build_exception_handler_inst_init (void * func);
struct inst * call_inst_init                    (int arg_count);
struct inst * compile_module_inst_init          (char * path);
struct inst * import_inst_init                  (char * file, struct vector * chain);
struct inst * iter_inst_init                    ();
struct inst * iter_full_inst_init               ();
struct inst * iter_next_inst_init               ();
struct inst * iter_free_inst_init               ();
struct inst * jump_inst_init                    (int label);
struct inst * jump_if_false_inst_init           (int label);
struct inst * jump_if_true_inst_init            (int label);
struct inst * list_decl_inst_init               (int count);
struct inst * load_attrib_inst_init             (char * attrib);
struct inst * load_global_inst_init             (int symbol);
struct inst * load_id_inst_init                 (char * id);
struct inst * load_local_inst_init              (int symbol);
struct inst * load_number_inst_init             (float f);
struct inst * load_string_inst_init             (char * str);
struct inst * load_subscript_inst_init          ();
struct inst * obj_decl_inst_init                (struct vector * ids);
//struct inst * obj_destructure_global_inst (struct vector * ids, struct int_vector * indices);
//struct inst * obj_destructure_local_inst (struct vector * ids, struct int_vector * indices);
struct inst * pop_inst_init                     ();
struct inst * pop_exception_handler_inst_init   ();
struct inst * store_attrib_inst_init            (char * attrib);
struct inst * store_global_inst_init            (int symbol);
struct inst * store_local_inst_init             (int symbol);
struct inst * store_subscript_inst_init         ();
struct inst * super_inst_init                   (int arg_count);
struct inst * typeof_inst_init                  ();
struct inst * unary_op_inst_init                (unary_op_type_t type);

#endif
