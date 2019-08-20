#include <emit/inst.h>

static struct inst * inst_init (inst_type_t type, void * state);

struct inst * bin_op_inst_init (bin_op_type_t type) {
    struct bin_op_inst * state;
    struct inst        * inst;

    state = (struct bin_op_inst *)calloc (1, sizeof (struct bin_op_inst));
    state->type = type;

    inst = inst_init (bin_op_inst, state);

    return inst;
}

struct inst * break_inst_init () {
    struct inst * inst;

    inst = inst_init (break_inst, NULL);

    return inst;
}

struct inst * call_inst_init (int arg_count) {
    struct call_inst * state;
    struct inst      * inst;

    state = (struct call_inst *)calloc (1, sizeof (struct call_inst));
    state->arg_count = arg_count;

    inst = inst_init (call_inst, state);

    return inst;
}

struct inst * compile_module_inst_init (char * file) {
    struct compile_module_inst * state;
    struct inst                * inst;

    state = (struct compile_module_inst *)calloc (1, sizeof (struct compile_module_inst));
    state->file = file;

    inst = inst_init (compile_module_inst, state);

    return inst;
}

struct inst * continue_inst_init () {
    struct inst * inst;

    inst = inst_init (continue_inst, NULL);

    return inst;
}

struct inst * import_inst_init (char * file, struct vector_state * name) {
    struct import_inst * state;
    struct inst        * inst;

    state = (struct import_inst *)calloc (1, sizeof (struct import_inst));
    state->file = file;
    state->name = name;

    inst = inst_init (import_inst, state);

    return inst;
}

struct inst * jump_inst_init (int label) {
    struct jump_inst * state;
    struct inst      * inst;

    state = (struct jump_inst *)calloc (1, sizeof (struct jump_inst));
    state->label = label;

    inst = inst_init (jump_inst, state);

    return inst;
}

struct inst * jump_if_true_inst_init (int label) {
    struct jump_if_true_inst * state;
    struct inst              * inst;

    state = (struct jump_if_true_inst *)calloc (1, sizeof (struct jump_if_true_inst));
    state->label = label;

    inst = inst_init (jump_if_true_inst, state);

    return inst;
}

struct inst * jump_if_false_inst_init (int label) {
    struct jump_if_false_inst * state;
    struct inst               * inst;

    state = (struct jump_if_false_inst *)calloc (1, sizeof (struct jump_if_false_inst));
    state->label = label;

    inst = inst_init (jump_if_false_inst, state);

    return inst;
}

struct inst * list_decl_inst_init (int count) {
    struct list_decl_inst * state;
    struct inst           * inst;

    state = (struct list_decl_inst *)calloc (1, sizeof (struct list_decl_inst));
    state->count = count;

    inst = inst_init (list_decl_inst, state);

    return inst;
}

struct inst * load_attrib_inst_init (char * attrib) {
    struct load_attrib_inst * state;
    struct inst             * inst;

    state = (struct load_attrib_inst *)calloc (1, sizeof (struct load_attrib_inst));
    state->attrib = attrib;

    inst = inst_init (load_attrib_inst, state);

    return inst;
}

struct inst * load_id_inst_init (int index, char * name) {
    struct load_id_inst * state;
    struct inst         * inst;

    state = (struct load_id_inst *)calloc (1, sizeof (struct load_id_inst));
    state->index = index;
    state->name  = name;

    inst = inst_init (load_id_inst, state);

    return inst;
}

struct inst * load_number_inst_init (float f) {
    struct load_number_inst * state;
    struct inst             * inst;

    state = (struct load_number_inst *)calloc (1, sizeof (struct load_number_inst));
    state->f = f;

    inst = inst_init (load_number_inst, state);

    return inst;
}

struct inst * load_string_inst_init (char * str) {
    struct load_string_inst * state;
    struct inst             * inst;

    state = (struct load_string_inst *)calloc (1, sizeof (struct load_string_inst));
    state->str = str;

    inst = inst_init (load_string_inst, state);

    return inst;
}

struct inst * obj_decl_inst_init (struct vector_state * ids) {
    struct obj_decl_inst * state;
    struct inst          * inst;

    state = (struct obj_decl_inst *)calloc (1, sizeof (struct obj_decl_inst));
    state->ids = ids;

    inst = inst_init (obj_decl_inst, state);

    return inst;
}

struct inst * obj_destructure_global_inst_init (struct vector_state * vars, struct vector_state * indices) {
    struct obj_destructure_global_inst * state;
    struct inst                        * inst;

    state = (struct obj_destructure_global_inst *)calloc (1, sizeof (struct obj_destructure_global_inst));
    state->vars    = vars;
    state->indices = indices;

    inst = inst_init (obj_destructure_global_inst, state);

    return inst;
}

struct inst * obj_destructure_local_inst_init (struct vector_state * vars, struct vector_state * indices) {
    struct obj_destructure_local_inst * state;
    struct inst                       * inst;

    state = (struct obj_destructure_local_inst *)calloc (1, sizeof (struct obj_destructure_local_inst));
    state->vars    = vars;
    state->indices = indices;

    inst = inst_init (obj_destructure_local_inst, state);

    return inst;
}

struct inst * pop_inst_init () {
    struct inst * inst;

    inst = inst_init (pop_inst, NULL);

    return inst;
}

struct inst * store_attrib_inst_init (char * attrib) {
    struct store_attrib_inst * state;
    struct inst              * inst;

    state = (struct store_attrib_inst *)calloc (1, sizeof (struct store_attrib_inst));
    state->attrib = attrib;

    inst = inst_init (store_attrib_inst, state);

    return inst;
}

struct inst * store_global_inst_init (int symbol) {
    struct store_global_inst * state;
    struct inst              * inst;

    state = (struct store_global_inst *)calloc (1, sizeof (struct store_global_inst));
    state->symbol = symbol;

    inst = inst_init (store_global_inst, state);

    return inst;
}

struct inst * store_local_inst_init (int symbol) {
    struct store_local_inst * state;
    struct inst             * inst;

    state = (struct store_local_inst *)calloc (1, sizeof (struct store_local_inst));
    state->symbol = symbol;

    inst = inst_init (store_local_inst, state);

    return inst;
}

struct inst * unary_op_inst_init (unary_op_type_t type) {
    struct unary_op_inst * state;
    struct inst          * inst;

    state = (struct unary_op_inst *)calloc (1, sizeof (struct unary_op_inst));
    state->type = type;

    inst = inst_init (unary_op_inst, state);

    return inst;
}

struct inst * use_global_inst_init (struct vector_state * ids, struct vector_state * indices) {
    struct use_global_inst * state;
    struct inst            * inst;

    state = (struct use_global_inst *)calloc (1, sizeof (struct use_global_inst));
    state->ids     = ids;
    state->indices = indices;

    inst = inst_init (use_global_inst, state);

    return inst;
}

struct inst * use_local_inst_init (struct vector_state * ids, struct vector_state * indices) {
    struct use_local_inst * state;
    struct inst           * inst;

    state = (struct use_local_inst *)calloc (1, sizeof (struct use_local_inst));
    state->ids     = ids;
    state->indices = indices;

    inst = inst_init (use_local_inst, state);

    return inst;
}

static struct inst * inst_init (inst_type_t type, void * state) {
    struct inst * inst;

    inst = (struct inst *)calloc (1, sizeof (struct inst));
    inst->type  = type;
    inst->state = state;

    return inst;
}
