#include <vm/vm.h>

static void import_module (struct vm_state * vm, struct has_obj * mod);

struct vm_state * vm_init (struct has_obj * mod) {
    struct vm_state * state;

    state                     = (struct vm_state *)calloc (1, sizeof (struct vm_state));
    state->mod                = mod;
    state->stack_frame        = stack_frame_init ();
    state->exception_returns  = vector_init ();
    state->exception_handlers = vector_init ();

    import_module (state, default_mod_init ());

    return state;
}

void vm_free (struct vm_state * state) {
    stack_frame_free (state->stack_frame);
    vector_free      (state->exception_returns);
    vector_free      (state->exception_handlers);

    free (state);
}

static struct run_state {
    struct vector_state * stack;
    struct inst         * inst;
    struct has_obj      * obj;
    int                   pos;
};

static void bin_op                  (struct vm_state * vm, struct run_state * run_state, struct bin_op_inst                  * state);
static void build_closure           (struct vm_state * vm, struct run_state * run_state, struct build_closure_inst           * state);
static void build_exception_handler (struct vm_state * vm, struct run_state * run_state, struct build_exception_handler_inst * state);
static void call                    (struct vm_state * vm, struct run_state * run_state, struct call_inst                    * state);
static void compile_module          (struct vm_state * vm, struct run_state * run_state, struct compile_module_inst          * state);
static void import                  (struct vm_state * vm, struct run_state * run_state, struct import_inst                  * state);
static void iter                    (struct vm_state * vm, struct run_state * run_state);
static void iter_full               (struct vm_state * vm, struct run_state * run_state);
static void iter_next               (struct vm_state * vm, struct run_state * run_state);
static void jump                    (struct vm_state * vm, struct run_state * run_state, struct jump_inst                    * state);
static void jump_if_true            (struct vm_state * vm, struct run_state * run_state, struct jump_if_true_inst            * state);
static void jump_if_false           (struct vm_state * vm, struct run_state * run_state, struct jump_if_false_inst           * state);
static void list_decl               (struct vm_state * vm, struct run_state * run_state, struct list_decl_inst               * state);
static void load_attrib             (struct vm_state * vm, struct run_state * run_state, struct load_attrib_inst             * state);
static void load_id                 (struct vm_state * vm, struct run_state * run_state, struct load_id_inst                 * state);
static void load_number             (struct vm_state * vm, struct run_state * run_state, struct load_number_inst             * state);
static void load_string             (struct vm_state * vm, struct run_state * run_state, struct load_string_inst             * state);
static void load_subscript          (struct vm_state * vm, struct run_state * run_state);
static void obj_decl                (struct vm_state * vm, struct run_state * run_state, struct obj_decl_inst                * state);
static void obj_destructure_global  (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_global_inst  * state);
static void obj_destructure_local   (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_local_inst   * state);
static void pop                     (struct vm_state * vm, struct run_state * run_state);
static void pop_exception_handler   (struct vm_state * vm, struct run_state * run_state);
static void raise                   (struct vm_state * vm, struct run_state * run_state);
static void self_reference          (struct vm_state * vm, struct run_state * run_state);
static void store_attrib            (struct vm_state * vm, struct run_state * run_state, struct store_attrib_inst            * state);
static void store_global            (struct vm_state * vm, struct run_state * run_state, struct store_global_inst            * state);
static void store_local             (struct vm_state * vm, struct run_state * run_state, struct store_local_inst             * state);
static void store_subscript         (struct vm_state * vm, struct run_state * run_state);
static void super                   (struct vm_state * vm, struct run_state * run_state, struct super_inst                   * state);
static void typeof                  (struct vm_state * vm, struct run_state * run_state);
static void unary_op                (struct vm_state * vm, struct run_state * run_state, struct unary_op_inst                * state);
static void use_global              (struct vm_state * vm, struct run_state * run_state, struct use_global_inst              * state);
static void use_local               (struct vm_state * vm, struct run_state * run_state, struct use_local_inst               * state);

struct has_obj * vm_run (struct vm_state * vm, struct has_obj * obj, struct has_obj * self) {
    struct run_state state;

    state.obj = obj;
    state.stack = vector_init ();

    state.pos = 0;
    while (state.pos < obj->instructions->length) {
        state.inst = vector_get (obj->instructions, state.pos);

        switch (state.inst->type) {
            case bin_op_inst:
                bin_op                     (vm, &state, (struct bin_op_inst *)                state.inst->state);
                break;
            case build_closure_inst:
                build_closure              (vm, &state, (struct build_closure_inst *)         state.inst->state);
                break;
            case build_exception_handler_inst:
                build_exception_handler    (vm, &state, (struct build_exception_handler_inst *) state.inst->state);
                break;
            case call_inst:
                call                       (vm, &state, (struct call_inst *)                    state.inst->state);
                break;
            case compile_module_inst:
                compile_module             (vm, &state, (struct compile_module_inst *)          state.inst->state);
                break;
            case import_inst:
                import                     (vm, &state, (struct import_inst *)                  state.inst->state);
                break;
            case iter_inst:
                iter                       (vm, &state);
                break;
            case iter_full_inst:
                iter_full                  (vm, &state);
                break;
            case iter_next_inst:
                iter_next                  (vm, &state);
                break;
            case jump_inst:
                jump                       (vm, &state, (struct jump_inst *)                    state.inst->state);
                break;
            case jump_if_true_inst:
                jump_if_true               (vm, &state, (struct jump_if_true_inst *)            state.inst->state);
                break;
            case jump_if_false_inst:
                jump_if_false              (vm, &state, (struct jump_if_false_inst *)           state.inst->state);
                break;
            case list_decl_inst:
                list_decl                  (vm, &state, (struct list_decl_inst *)               state.inst->state);
                break;
            case load_attrib_inst:
                load_attrib                (vm, &state, (struct load_attrib_inst *)             state.inst->state);
                break;
            case load_id_inst:
                load_id                    (vm, &state, (struct load_id_inst *)                 state.inst->state);
                break;
            case load_number_inst:
                load_number                (vm, &state, (struct load_number_inst *)             state.inst->state);
                break;
            case load_string_inst:
                load_string                (vm, &state, (struct load_string_inst *)             state.inst->state);
                break;
            case load_subscript_inst:
                load_subscript             (vm, &state);
                break;
            case obj_decl_inst:
                obj_decl                   (vm, &state, (struct obj_decl_inst *)                state.inst->state);
                break;
            case obj_destructure_global_inst:
                obj_destructure_global     (vm, &state, (struct obj_destructure_global_inst *)  state.inst->state);
                break;
            case obj_destructure_local_inst:
                obj_destructure_local      (vm, &state, (struct obj_destructure_local_inst *)   state.inst->state);
                break;
            case pop_inst:
                pop                        (vm, &state);
                break;
            case pop_exception_handler_inst:
                pop_exception_handler      (vm, &state);
                break;
            case raise_inst:
                raise                      (vm, &state);
                break;
            case return_inst:
                return vector_pop          (state.stack);
            case self_reference_inst:
                self_reference             (vm, &state);
                break;
            case store_attrib_inst:
                store_attrib               (vm, &state, (struct store_attrib_inst *)            state.inst->state);
                break;
            case store_global_inst:
                store_global               (vm, &state, (struct store_global_inst *)            state.inst->state);
                break;
            case store_local_inst:
                store_local                (vm, &state, (struct store_local_inst *)             state.inst->state);
                break;
            case store_subscript_inst:
                store_subscript            (vm, &state);
                break;
            case super_inst:
                super                      (vm, &state, (struct super_inst *)                   state.inst->state);
                break;
            case typeof_inst:
                typeof                     (vm, &state);
                break;
            case unary_op_inst:
                unary_op                   (vm, &state, (struct unary_op_inst *)                state.inst->state);
                break;
            case use_global_inst:
                use_global                 (vm, &state, (struct use_global_inst *)              state.inst->state);
                break;
            case use_local_inst:
                use_local                  (vm, &state, (struct use_local_inst *)               state.inst->state);
                break;
        }

        state.pos++;
    }

    vector_free (state.stack);
}

static void bin_op (struct vm_state * vm, struct run_state * run_state, struct bin_op_inst * state) {

}

static void build_closure (struct vm_state * vm, struct run_state * run_state, struct build_closure_inst * state) {

}

static void build_exception_handler (struct vm_state * vm, struct run_state * run_state, struct build_exception_handler_inst * state) {

}

static void call (struct vm_state * vm, struct run_state * run_state, struct call_inst * state) {

}

static void compile_module (struct vm_state * vm, struct run_state * run_state, struct compile_module_inst * state) {

}

static void import                  (struct vm_state * vm, struct run_state * run_state, struct import_inst                  * state) {

}

static void iter                    (struct vm_state * vm, struct run_state * run_state) {

}

static void iter_full               (struct vm_state * vm, struct run_state * run_state) {

}

static void iter_next               (struct vm_state * vm, struct run_state * run_state) {

}

static void jump                    (struct vm_state * vm, struct run_state * run_state, struct jump_inst                    * state) {

}

static void jump_if_true            (struct vm_state * vm, struct run_state * run_state, struct jump_if_true_inst            * state) {

}

static void jump_if_false           (struct vm_state * vm, struct run_state * run_state, struct jump_if_false_inst           * state) {

}

static void list_decl               (struct vm_state * vm, struct run_state * run_state, struct list_decl_inst               * state) {

}

static void load_attrib             (struct vm_state * vm, struct run_state * run_state, struct load_attrib_inst             * state) {

}

static void load_id                 (struct vm_state * vm, struct run_state * run_state, struct load_id_inst                 * state) {

}

static void load_number             (struct vm_state * vm, struct run_state * run_state, struct load_number_inst             * state) {

}

static void load_string             (struct vm_state * vm, struct run_state * run_state, struct load_string_inst             * state) {

}

static void load_subscript          (struct vm_state * vm, struct run_state * run_state) {

}

static void obj_decl                (struct vm_state * vm, struct run_state * run_state, struct obj_decl_inst                * state) {

}

static void obj_destructure_global  (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_global_inst  * state) {

}

static void obj_destructure_local   (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_local_inst   * state) {

}

static void pop                     (struct vm_state * vm, struct run_state * run_state) {

}

static void pop_exception_handler   (struct vm_state * vm, struct run_state * run_state) {

}

static void raise                   (struct vm_state * vm, struct run_state * run_state) {

}

static void self_reference          (struct vm_state * vm, struct run_state * run_state) {

}

static void store_attrib            (struct vm_state * vm, struct run_state * run_state, struct store_attrib_inst            * state) {

}

static void store_global            (struct vm_state * vm, struct run_state * run_state, struct store_global_inst            * state) {

}

static void store_local             (struct vm_state * vm, struct run_state * run_state, struct store_local_inst             * state) {

}

static void store_subscript         (struct vm_state * vm, struct run_state * run_state) {

}

static void super                   (struct vm_state * vm, struct run_state * run_state, struct super_inst                   * state) {

}

static void typeof                  (struct vm_state * vm, struct run_state * run_state) {

}

static void unary_op                (struct vm_state * vm, struct run_state * run_state, struct unary_op_inst                * state) {

}

static void use_global              (struct vm_state * vm, struct run_state * run_state, struct use_global_inst              * state) {

}

static void use_local               (struct vm_state * vm, struct run_state * run_state, struct use_local_inst               * state) {

}

static void import_module (struct vm_state * vm, struct has_obj * mod) {
    char * key;

    for (int i = 0; i < mod->attribs->keys->length; i++) {
        key = vector_get (mod->attribs->keys, i);
        has_obj_set_attrib (vm->mod, key, vector_get (mod->attribs->vals, i));
    }
}
