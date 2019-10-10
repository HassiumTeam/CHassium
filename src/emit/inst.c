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
