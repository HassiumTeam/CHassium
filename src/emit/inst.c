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

struct inst * bin_op_inst_init (bin_op_type_t type) {}
struct inst * build_closure_inst_init (void * func) {}
struct inst * build_exception_handler_inst_init (void * func) {}
struct inst * call_inst_init (int arg_count) {}
struct inst * compile_module_inst_init (char * path) {}
struct inst * import_inst_init (char * file, struct vector * chain) {}
struct inst * iter_inst_init () {}
struct inst * iter_full_inst_init () {}
struct inst * iter_next_inst_init () {}
struct inst * iter_free_inst_init () {}
struct inst * jump_inst_init (int label) {}
struct inst * jump_if_false_inst_init (int label) {}
struct inst * jump_if_true_inst_init (int label) {}
struct inst * list_decl_inst_init (int count) {}
struct inst * load_attrib_inst_init (char * attrib) {}
struct inst * load_global_inst_init (int symbol) {}
struct inst * load_id_inst_init (char * id) {}
struct inst * load_local_inst_init (int symbol) {}
struct inst * load_number_inst_init (float f) {}
struct inst * load_string_inst_init (char * str) {}
struct inst * load_subscript_inst_init () {}
struct inst * obj_decl_inst_init (struct vector * ids) {}
//struct inst * obj_destructure_global_inst (struct vector * ids, struct int_vector * indices);
//struct inst * obj_destructure_local_inst (struct vector * ids, struct int_vector * indices);
struct inst * pop_inst_init () {}
struct inst * pop_exception_handler_inst_init () {}
struct inst * store_attrib_inst_init (char * attrib) {}
struct inst * store_global_inst_init (int symbol) {}
struct inst * store_local_inst_init (int symbol) {}
struct inst * store_subscript_inst_init () {}
struct inst * super_inst_init (int arg_count) {}
struct inst * typeof_inst_init () {}
struct inst * unary_op_inst_init (unary_op_type_t type) {}
