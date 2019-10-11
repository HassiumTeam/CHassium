#include <emit/inst.h>

struct inst * inst_init (inst_type_t type) {
    struct inst * inst;

    inst       = (struct inst *)calloc (1, sizeof (struct inst));
    inst->type = type;

    return inst;
}

void free_inst (struct inst * inst) {
    switch (inst->type) {
        case BUILD_CLOSURE_INST:
        case BUILD_EXCEPTION_HANDLER_INST:
            free_has_obj (inst->arg5);
            break;
        case COMPILE_MODULE_INST:
        case LOAD_ATTRIB_INST:
        case LOAD_ID_INST:
        case LOAD_STRING_INST:
        case STORE_ATTRIB_INST:
            free (inst->arg4);
            break;
        case IMPORT_INST:
            if (inst->arg4) {
                free (inst->arg4);
            } else {
                free_vector (inst->arg5);
            }
            break;
        case OBJ_DECL_INST:
            free_string_vector (inst->arg5);
            break;
        case OBJ_DESTRUCTURE_LOCAL_INST:
        case OBJ_DESTRUCTURE_GLOBAL_INST:
            // TODO
            break;
    }

    free (inst);
}

struct inst * bin_op_inst_init (bin_op_type_t type) {
    struct inst * inst;

    inst       = inst_init (BIN_OP_INST);
    inst->arg1 = type;

    return inst;
}

struct inst * build_closure_inst_init (void * func) {
    struct inst * inst;

    inst       = inst_init (BUILD_CLOSURE_INST);
    inst->arg5 = func;

    return inst;
}

struct inst * build_exception_handler_inst_init (void * func) {
    struct inst * inst;

    inst       = inst_init (BUILD_EXCEPTION_HANDLER_INST);
    inst->arg5 = func;

    return inst;
}

struct inst * call_inst_init (int arg_count) {
    struct inst * inst;

    inst       = inst_init (CALL_INST);
    inst->arg1 = arg_count;

    return inst;
}

struct inst * compile_module_inst_init (char * path) {
    struct inst * inst;

    inst       = inst_init (COMPILE_MODULE_INST);
    inst->arg4 = path;

    return inst;
}

struct inst * import_inst_init (char * file, struct vector * chain) {
    struct inst * inst;

    inst       = inst_init (IMPORT_INST);
    inst->arg4 = file;
    inst->arg5 = chain;

    return inst;
}

struct inst * iter_inst_init () {
    struct inst * inst;

    inst = inst_init (ITER_INST);

    return inst;
}

struct inst * iter_full_inst_init () {
    struct inst * inst;

    inst = inst_init (ITER_FULL_INST);

    return inst;
}

struct inst * iter_next_inst_init () {
    struct inst * inst;

    inst = inst_init (ITER_NEXT_INST);

    return inst;
}

struct inst * iter_free_inst_init () {
    struct inst * inst;

    inst = inst_init (ITER_FREE_INST);

    return inst;
}

struct inst * jump_inst_init (int label) {
    struct inst * inst;

    inst       = inst_init (JUMP_INST);
    inst->arg1 = label;

    return inst;
}

struct inst * jump_if_false_inst_init (int label) {
    struct inst * inst;

    inst       = inst_init (JUMP_IF_FALSE_INST);
    inst->arg1 = label;

    return inst;
}

struct inst * jump_if_true_inst_init (int label) {
    struct inst * inst;

    inst       = inst_init (JUMP_IF_TRUE_INST);
    inst->arg1 = label;

    return inst;
}

struct inst * list_decl_inst_init (int count) {
    struct inst * inst;

    inst       = inst_init (LIST_DECL_INST);
    inst->arg1 = count;

    return inst;
}

struct inst * load_attrib_inst_init (char * attrib) {
    struct inst * inst;

    inst       = inst_init (LOAD_ATTRIB_INST);
    inst->arg4 = attrib;

    return inst;
}

struct inst * load_global_inst_init (int symbol) {
    struct inst * inst;

    inst       = inst_init (LOAD_GLOBAL_INST);
    inst->arg1 = symbol;

    return inst;
}

struct inst * load_id_inst_init (char * id) {
    struct inst * inst;

    inst       = inst_init (LOAD_ID_INST);
    inst->arg4 = id;

    return inst;
}

struct inst * load_local_inst_init (int symbol) {
    struct inst * inst;

    inst       = inst_init (LOAD_LOCAL_INST);
    inst->arg1 = symbol;

    return inst;
}

struct inst * load_number_inst_init (float f) {
    struct inst * inst;

    inst       = inst_init (LOAD_NUMBER_INST);
    inst->arg3 = f;

    return inst;
}

struct inst * load_string_inst_init (char * str) {
    struct inst * inst;

    inst       = inst_init (LOAD_STRING_INST);
    inst->arg4 = str;

    return inst;
}

struct inst * load_subscript_inst_init () {
    struct inst * inst;

    inst = inst_init (LOAD_SUBSCRIPT_INST);

    return inst;
}

struct inst * obj_decl_inst_init (struct vector * ids) {
    struct inst * inst;

    inst       = inst_init (OBJ_DECL_INST);
    inst->arg5 = ids;

    return inst;
}
//struct inst * obj_destructure_global_inst (struct vector * ids, struct int_vector * indices);
//struct inst * obj_destructure_local_inst (struct vector * ids, struct int_vector * indices);
struct inst * pop_inst_init () {
    struct inst * inst;

    inst = inst_init (POP_INST);

    return inst;
}

struct inst * pop_exception_handler_inst_init () {
    struct inst * inst;

    inst = inst_init (POP_EXCEPTION_HANDLER_INST);

    return inst;
}

struct inst * store_attrib_inst_init (char * attrib) {
    struct inst * inst;

    inst       = inst_init (STORE_ATTRIB_INST);
    inst->arg4 = attrib;

    return inst;
}

struct inst * store_global_inst_init (int symbol) {
    struct inst * inst;

    inst       = inst_init (STORE_GLOBAL_INST);
    inst->arg1 = symbol;

    return inst;
}

struct inst * store_local_inst_init (int symbol) {
    struct inst * inst;

    inst       = inst_init (STORE_LOCAL_INST);
    inst->arg1 = symbol;

    return inst;
}

struct inst * store_subscript_inst_init () {
    struct inst * inst;

    inst = inst_init (STORE_SUBSCRIPT_INST);

    return inst;
}

struct inst * super_inst_init (int arg_count) {
    struct inst * inst;

    inst       = inst_init (SUPER_INST);
    inst->arg1 = arg_count;

    return inst;
}

struct inst * typeof_inst_init () {
    struct inst * inst;

    inst = inst_init (TYPEOF_INST);

    return inst;
}

struct inst * unary_op_inst_init (unary_op_type_t type) {
    struct inst * inst;

    inst       = inst_init (UNARY_OP_INST);
    inst->type = type;

    return inst;
}
