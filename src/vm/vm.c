#include <vm/vm.h>

struct vm_state * vm_init (struct has_obj * mod) {
    struct vm_state * state;

    state                     = (struct vm_state *)calloc (1, sizeof (struct vm_state));
    state->mod                = mod;
    state->stack_frame        = stack_frame_init ();
    state->exception_returns  = vector_init ();
    state->exception_handlers = vector_init ();

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
                bin_op (vm, &state, (struct bin_op_inst *)state.inst->state);
                break;
            case build_closure_inst:
                build_closure (vm, &state, (struct build_closure_inst *)state.inst->state);
                break;
            case build_exception_handler_inst:
                build_exception_handler (vm, &state, (struct build_exception_handler_inst *)state.inst->state);
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
