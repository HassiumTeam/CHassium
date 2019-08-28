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

struct inst * build_closure_inst_init (struct has_obj * func) {
    struct build_closure_inst * state;
    struct inst               * inst;

    state       = (struct build_closure_inst *)calloc (1, sizeof (struct build_closure_inst));
    state->func = func;

    inst = inst_init (build_closure_inst, state);

    return inst;
}

struct inst * build_exception_handler_inst_init (struct has_obj * func, int caught_label) {
    struct build_exception_handler_inst * state;
    struct inst                         * inst;

    state               = (struct build_exception_handler_inst *)calloc (1, sizeof (struct build_exception_handler_inst));
    state->func         = func;
    state->caught_label = caught_label;

    inst = inst_init (build_exception_handler_inst, state);

    return inst;
}

struct inst * call_inst_init (int arg_count) {
    struct call_inst * state;
    struct inst      * inst;

    state            = (struct call_inst *)calloc (1, sizeof (struct call_inst));
    state->arg_count = arg_count;

    inst = inst_init (call_inst, state);

    return inst;
}

struct inst * compile_module_inst_init (char * file) {
    struct compile_module_inst * state;
    struct inst                * inst;

    state       = (struct compile_module_inst *)calloc (1, sizeof (struct compile_module_inst));
    state->file = file;

    inst = inst_init (compile_module_inst, state);

    return inst;
}

struct inst * import_inst_init (char * file, struct vector_state * name) {
    struct import_inst * state;
    struct inst        * inst;

    state = (struct import_inst *)calloc (1, sizeof (struct import_inst));
    if (file) {
        state->file = (char *)calloc (strlen (file) + 1, sizeof (char));
        memcpy (state->file, file, strlen (file) + 1);
    }
    if (name) {
        state->name = (struct vector_state *)calloc (1, sizeof (struct vector_state));
        memcpy (state->name, name, sizeof (struct vector_state));
    }

    inst = inst_init (import_inst, state);

    return inst;
}

struct inst * iter_inst_init () {
    struct inst * inst;

    inst = inst_init (iter_inst, NULL);

    return inst;
}

struct inst * iter_full_inst_init () {
    struct inst * inst;

    inst = inst_init (iter_full_inst, NULL);

    return inst;
}

struct inst * iter_free_inst_init () {
    struct inst * inst;

    inst = inst_init (iter_free_inst, NULL);
}

struct inst * iter_next_inst_init () {
    struct inst * inst;

    inst = inst_init (iter_next_inst, NULL);

    return inst;
}

struct inst * jump_inst_init (int label) {
    struct jump_inst * state;
    struct inst      * inst;

    state        = (struct jump_inst *)calloc (1, sizeof (struct jump_inst));
    state->label = label;

    inst = inst_init (jump_inst, state);

    return inst;
}

struct inst * jump_if_true_inst_init (int label) {
    struct jump_if_true_inst * state;
    struct inst              * inst;

    state        = (struct jump_if_true_inst *)calloc (1, sizeof (struct jump_if_true_inst));
    state->label = label;

    inst = inst_init (jump_if_true_inst, state);

    return inst;
}

struct inst * jump_if_false_inst_init (int label) {
    struct jump_if_false_inst * state;
    struct inst               * inst;

    state        = (struct jump_if_false_inst *)calloc (1, sizeof (struct jump_if_false_inst));
    state->label = label;

    inst = inst_init (jump_if_false_inst, state);

    return inst;
}

struct inst * list_decl_inst_init (int count) {
    struct list_decl_inst * state;
    struct inst           * inst;

    state        = (struct list_decl_inst *)calloc (1, sizeof (struct list_decl_inst));
    state->count = count;

    inst = inst_init (list_decl_inst, state);

    return inst;
}

struct inst * load_attrib_inst_init (char * attrib) {
    struct load_attrib_inst * state;
    struct inst             * inst;

    state         = (struct load_attrib_inst *)calloc (1, sizeof (struct load_attrib_inst));
    state->attrib = (char *)calloc (strlen (attrib) + 1, sizeof (char));

    memcpy (state->attrib, attrib, strlen (attrib) + 1);

    inst = inst_init (load_attrib_inst, state);

    return inst;
}

struct inst * load_id_inst_init (int index, char * name) {
    struct load_id_inst * state;
    struct inst         * inst;

    state        = (struct load_id_inst *)calloc (1, sizeof (struct load_id_inst));
    state->index = index;
    if (name) {
        state->name = (char *)calloc (strlen (name) + 1, sizeof (char));
        memcpy (state->name, name, strlen (name) + 1);
    }

    inst = inst_init (load_id_inst, state);

    return inst;
}

struct inst * load_number_inst_init (float f) {
    struct load_number_inst * state;
    struct inst             * inst;

    state    = (struct load_number_inst *)calloc (1, sizeof (struct load_number_inst));
    state->f = f;

    inst = inst_init (load_number_inst, state);

    return inst;
}

struct inst * load_string_inst_init (char * str) {
    struct load_string_inst * state;
    struct inst             * inst;

    state      = (struct load_string_inst *)calloc (1, sizeof (struct load_string_inst));
    state->str = (char *)calloc (strlen (str) + 1, sizeof (char));

    memcpy (state->str, str, strlen (str) + 1);

    inst = inst_init (load_string_inst, state);

    return inst;
}

struct inst * load_subscript_inst_init () {
    struct inst * inst;

    inst = inst_init (load_subscript_inst, NULL);

    return inst;
}

struct inst * obj_decl_inst_init (struct vector_state * ids) {
    struct obj_decl_inst * state;
    struct inst          * inst;

    state      = (struct obj_decl_inst *)calloc (1, sizeof (struct obj_decl_inst));
    state->ids = ids;

    inst = inst_init (obj_decl_inst, state);

    return inst;
}

struct inst * obj_destructure_global_inst_init (struct vector_state * vars, struct int_vector_state * indices) {
    struct obj_destructure_global_inst * state;
    struct inst                        * inst;

    state          = (struct obj_destructure_global_inst *)calloc (1, sizeof (struct obj_destructure_global_inst));
    state->vars    = vars;
    state->indices = indices;

    inst = inst_init (obj_destructure_global_inst, state);

    return inst;
}

struct inst * obj_destructure_local_inst_init (struct vector_state * vars, struct int_vector_state * indices) {
    struct obj_destructure_local_inst * state;
    struct inst                       * inst;

    state          = (struct obj_destructure_local_inst *)calloc (1, sizeof (struct obj_destructure_local_inst));
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

struct inst * pop_exception_handler_inst_init () {
    struct inst * inst;

    inst = inst_init (pop_exception_handler_inst, NULL);

    return inst;
}

struct inst * raise_inst_init () {
    struct inst * inst;

    inst = inst_init (raise_inst, NULL);

    return inst;
}

struct inst * return_inst_init () {
    struct inst * inst;

    inst = inst_init (return_inst, NULL);

    return inst;
}

struct inst * self_reference_inst_init () {
    struct inst * inst;

    inst = inst_init (self_reference_inst, NULL);

    return inst;
}

struct inst * store_attrib_inst_init (char * attrib) {
    struct store_attrib_inst * state;
    struct inst              * inst;

    state         = (struct store_attrib_inst *)calloc (1, sizeof (struct store_attrib_inst));
    state->attrib = (char *)calloc (strlen (attrib) + 1, sizeof (char));

    memcpy (state->attrib, attrib, strlen (attrib) + 1);

    inst = inst_init (store_attrib_inst, state);

    return inst;
}

struct inst * store_global_inst_init (int symbol) {
    struct store_global_inst * state;
    struct inst              * inst;

    state         = (struct store_global_inst *)calloc (1, sizeof (struct store_global_inst));
    state->symbol = symbol;

    inst = inst_init (store_global_inst, state);

    return inst;
}

struct inst * store_local_inst_init (int symbol) {
    struct store_local_inst * state;
    struct inst             * inst;

    state         = (struct store_local_inst *)calloc (1, sizeof (struct store_local_inst));
    state->symbol = symbol;

    inst = inst_init (store_local_inst, state);

    return inst;
}

struct inst * store_subscript_inst_init () {
    struct inst * inst;

    inst = inst_init (store_subscript_inst, NULL);

    return inst;
}

struct inst * super_inst_init (int arg_count) {
    struct super_inst * state;
    struct inst       * inst;

    state            = (struct super_inst *)calloc (1, sizeof (struct super_inst));
    state->arg_count = arg_count;

    inst = inst_init (super_inst, state);

    return inst;
}

struct inst * typeof_inst_init () {
    struct inst * inst;

    inst = inst_init (typeof_inst, NULL);

    return inst;
}

struct inst * unary_op_inst_init (unary_op_type_t type) {
    struct unary_op_inst * state;
    struct inst          * inst;

    state       = (struct unary_op_inst *)calloc (1, sizeof (struct unary_op_inst));
    state->type = type;

    inst = inst_init (unary_op_inst, state);

    return inst;
}

struct inst * use_global_inst_init (struct vector_state * ids, struct vector_state * indices) {
    struct use_global_inst * state;
    struct inst            * inst;

    state          = (struct use_global_inst *)calloc (1, sizeof (struct use_global_inst));
    state->ids     = (struct vector_state *)   calloc (1, sizeof (struct vector_state));
    state->indices = indices;

    memcpy (state->ids, ids, sizeof (struct vector_state));

    inst = inst_init (use_global_inst, state);

    return inst;
}

struct inst * use_local_inst_init (struct vector_state * ids, struct vector_state * indices) {
    struct use_local_inst * state;
    struct inst           * inst;

    state          = (struct use_local_inst *)calloc (1, sizeof (struct use_local_inst));
    state->ids     = (struct vector_state *)calloc (1, sizeof (struct vector_state));
    state->indices = indices;

    memcpy (state->ids, ids, sizeof (struct vector_state));

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

void inst_free (struct inst * inst) {
    switch (inst->type) {
        case build_closure_inst:
            build_closure_inst_free ((struct build_closure_inst *)inst->state);
            break;
        case build_exception_handler_inst:
            build_exception_handler_inst_free ((struct build_exception_handler_inst *)inst->state);
            break;
        case compile_module_inst:
            compile_module_inst_free ((struct compile_module_inst *)inst->state);
            break;
        case import_inst:
            import_inst_free ((struct import_inst *)inst->state);
            break;
        case load_attrib_inst:
            load_attrib_inst_free ((struct load_attrib_inst *)inst->state);
            break;
        case load_id_inst:
            load_id_inst_free ((struct load_id_inst *)inst->state);
            break;
        case load_string_inst:
            load_string_inst_free ((struct load_string_inst *)inst->state);
            break;
        case obj_decl_inst:
            obj_decl_inst_free ((struct obj_decl_inst *)inst->state);
            break;
        case obj_destructure_global_inst:
            obj_destructure_global_inst_free ((struct obj_destructure_global_inst *)inst->state);
            break;
        case obj_destructure_local_inst:
            obj_destructure_local_inst_free ((struct obj_destructure_local_inst *)inst->state);
            break;
        case store_attrib_inst:
            store_attrib_inst_free ((struct store_attrib_inst *)inst->state);
            break;
        case use_global_inst:
            use_global_inst_free ((struct use_global_inst *)inst->state);
            break;
        case use_local_inst:
            use_local_inst_free ((struct use_local_inst *)inst->state);
            break;
        defaut:
            break;
    }

    if (inst->state) {
        free (inst->state);
    }
    free (inst);
}

static void access_chain_free (struct vector_state * chain);

void build_closure_inst_free (struct build_closure_inst * inst) {
}

void build_exception_handler_inst_free (struct build_exception_handler_inst * inst) {
}

void compile_module_inst_free (struct compile_module_inst * inst) {
    free (inst->file);
}

void import_inst_free (struct import_inst * inst) {
    if (inst->file) {
        free (inst->file);
    }
    if (inst->name) {
        access_chain_free (inst->name);
    }
}

void load_attrib_inst_free (struct load_attrib_inst * inst) {
    free (inst->attrib);
}

void load_id_inst_free (struct load_id_inst * inst) {
    if (inst->name) {
        free (inst->name);
    }
}

void load_string_inst_free (struct load_string_inst * inst) {
    free (inst->str);
}

void obj_decl_inst_free (struct obj_decl_inst * inst) {
    access_chain_free (inst->ids);
}

void obj_destructure_global_inst_free (struct obj_destructure_global_inst * inst) {
    access_chain_free (inst->vars);
    int_vector_free   (inst->indices);
}

void obj_destructure_local_inst_free (struct obj_destructure_local_inst * inst) {
    access_chain_free (inst->vars);
    int_vector_free   (inst->indices);
}

void store_attrib_inst_free (struct store_attrib_inst * inst) {
    free (inst->attrib);
}

void use_global_inst_free (struct use_global_inst * inst) {
    access_chain_free (inst->ids);
    vector_free   (inst->indices);
}

void use_local_inst_free (struct use_local_inst * inst) {
    access_chain_free (inst->ids);
    vector_free   (inst->indices);
}

static void access_chain_free (struct vector_state * chain) {
    for (int i = 0; i < chain->length; i++) {
        free (vector_get (chain, i));
    }

    vector_free (chain);
}
