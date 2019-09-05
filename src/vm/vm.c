#include <vm/vm.h>

static void import_module (struct vm_state * vm, struct has_obj * mod);

struct vm_state * vm_init (struct has_obj * mod) {
    struct vm_state * state;

    state                     = (struct vm_state *)calloc (1, sizeof (struct vm_state));
    state->mod                = mod;
    state->stack_frame        = stack_frame_init ();
    state->exception_returns  = vector_init ();
    state->exception_handlers = vector_init ();

    import_module (state, get_default_mod ());

    return state;
}

void vm_free (struct vm_state * state) {
    gc_remove_ref    (state->mod);
    gc_remove_ref    (get_default_mod ());

    stack_frame_free (state->stack_frame);
    vector_free      (state->exception_returns);
    vector_free      (state->exception_handlers);

    free (state);
}

static void import_module (struct vm_state * vm, struct has_obj * mod) {
    char * key;

    for (int i = 0; i < mod->attribs->keys->length; i++) {
        key = vector_get (mod->attribs->keys, i);
        has_obj_set_attrib (vm->mod, key, vector_get (mod->attribs->vals, i));
    }
}

static struct run_state {
    struct vector_state * stack;
    struct inst         * inst;
    struct has_obj      * obj;
    struct has_obj      * self;
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
static void iter_free               (struct vm_state * vm, struct run_state * run_state);
static void iter_next               (struct vm_state * vm, struct run_state * run_state);
static void jump                    (struct vm_state * vm, struct run_state * run_state, struct jump_inst                    * state);
static void jump_if_true            (struct vm_state * vm, struct run_state * run_state, struct jump_if_true_inst            * state);
static void jump_if_false           (struct vm_state * vm, struct run_state * run_state, struct jump_if_false_inst           * state);
static void list_decl               (struct vm_state * vm, struct run_state * run_state, struct list_decl_inst               * state);
static void load_attrib             (struct vm_state * vm, struct run_state * run_state, struct load_attrib_inst             * state);
static void load_global             (struct vm_state * vm, struct run_state * run_state, struct load_global_inst             * state);
static void load_id                 (struct vm_state * vm, struct run_state * run_state, struct load_id_inst                 * state);
static void load_local              (struct vm_state * vm, struct run_state * run_state, struct load_local_inst              * state);
static void load_number             (struct vm_state * vm, struct run_state * run_state, struct load_number_inst             * state);
static void load_string             (struct vm_state * vm, struct run_state * run_state, struct load_string_inst             * state);
static void load_subscript          (struct vm_state * vm, struct run_state * run_state);
static void obj_decl                (struct vm_state * vm, struct run_state * run_state, struct obj_decl_inst                * state);
static void obj_destructure_global  (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_global_inst  * state);
static void obj_destructure_local   (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_local_inst   * state);
static void pop_                    (struct vm_state * vm, struct run_state * run_state);
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
    struct has_obj * ret;

    state.obj   = obj;
    state.self  = self;
    state.stack = vector_init ();
    state.pos   = 0;

    if (state.self != NULL) {
        gc_add_ref (state.self);
    }

    while (state.pos < obj->instructions->length) {
        state.inst = vector_get (obj->instructions, state.pos);
        printf("%d\n", state.inst->type);
        switch (state.inst->type) {
            case bin_op_inst:
                bin_op                     (vm, &state, (struct bin_op_inst *)                 state.inst->state);
                break;
            case build_closure_inst:
                build_closure              (vm, &state, (struct build_closure_inst *)          state.inst->state);
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
            case iter_free_inst:
                iter_free                  (vm, &state);
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
            case load_global_inst:
                load_global                (vm, &state, (struct load_global_inst *)             state.inst->state);
                break;
            case load_id_inst:
                load_id                    (vm, &state, (struct load_id_inst *)                 state.inst->state);
                break;
            case load_local_inst:
                load_local                 (vm, &state, (struct load_local_inst *)              state.inst->state);
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
                pop_                       (vm, &state);
                break;
            case pop_exception_handler_inst:
                pop_exception_handler      (vm, &state);
                break;
            case raise_inst:
                raise                      (vm, &state);
                break;
            case return_inst:
                ret = vector_pop (state.stack);
                vector_free (state.stack);

                if (state.self != NULL) {
                    gc_remove_ref (state.self);
                }
                //ret->ref_count--;
                return ret;
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

    if (state.self != NULL) {
        gc_remove_ref (state.self);
    }

    return has_obj_get_attrib (get_default_mod (), "null");
}

struct has_obj * vm_resolve_access_chain (struct vm_state * vm, struct vector_state * chain) {
    struct has_obj * top;
    char           * link;

    link = vector_get         (chain, 0);
    top  = has_obj_get_attrib (vm->mod, link);

    for (int i = 1; i < chain->length; i++) {
        link = vector_get (chain, i);
        top = has_obj_get_attrib (top, link);
    }

    return top;
}

static void bin_op (struct vm_state * vm, struct run_state * run_state, struct bin_op_inst * state) {
    struct has_obj * right;
    struct has_obj * left;

    left  = vector_pop (run_state->stack);
    right = vector_pop (run_state->stack);

    switch (state->type) {
        case add_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_add              (vm, left, right)));
            break;
        case bitwise_and_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_bitwise_and      (vm, left, right)));
            break;
        case bitwise_or_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_bitwise_or       (vm, left, right)));
            break;
        case div_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_divide           (vm, left, right)));
            break;
        case equal_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_equal            (vm, left, right)));
            break;
        case greater_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_greater          (vm, left, right)));
            break;
        case greater_or_equal_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_greater_or_equal (vm, left, right)));
            break;
        case instanceof_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_instanceof       (vm, left, right)));
            break;
        case lesser_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_lesser           (vm, left, right)));
            break;
        case lesser_or_equal_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_lesser_or_equal  (vm, left, right)));
            break;
        case logical_and_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_logical_and      (vm, left, right)));
            break;
        case logical_or_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_logical_or       (vm, left, right)));
            break;
        case mod_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_mod              (vm, left, right)));
            break;
        case mul_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_mul              (vm, left, right)));
            break;
        case sub_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_sub              (vm, left, right)));
            break;
        case xor_bin_op:
            vector_push (run_state->stack, gc_add_ref (has_obj_xor              (vm, left, right)));
            break;
    }

    gc_remove_ref (left);
    gc_remove_ref (right);
}

static void build_closure (struct vm_state * vm, struct run_state * run_state, struct build_closure_inst * state) {
    vector_push (run_state->stack, gc_add_ref (has_closure_init (state->func, peek_frame (vm->stack_frame))));
}

static void build_exception_handler (struct vm_state * vm, struct run_state * run_state, struct build_exception_handler_inst * state) {

}

static void call (struct vm_state * vm, struct run_state * run_state, struct call_inst * state) {
    struct vector_state * args;
    struct has_obj      * target;

    target = vector_pop (run_state->stack);

    args   = vector_init ();
    for (int i = 0; i < state->arg_count; i++) {
        vector_push (args, vector_pop (run_state->stack));
    }

    vector_push (run_state->stack, gc_add_ref (has_obj_invoke (vm, target, args)));

    gc_remove_vect (args);
    gc_remove_ref  (target);

    vector_free (args);
}

static void compile_module (struct vm_state * vm, struct run_state * run_state, struct compile_module_inst * state) {
    vector_push (run_state->stack, gc_add_ref (compile (state->file)));
}

static void import (struct vm_state * vm, struct run_state * run_state, struct import_inst * state) {
    struct has_obj * mod;

    if (state->name != NULL) {
        mod = vm_resolve_access_chain (vm, state->name);
    } else {
        mod = compile (state->file);
    }
    import_module (vm, mod);

    gc_remove_ref (mod);
}

static void iter (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * obj;

    obj = vector_pop (run_state->stack);

    vector_push (run_state->stack, has_obj_iter (vm, obj));

    gc_remove_ref (obj);
}

static void iter_free (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * iterable;

    iterable = vector_pop (run_state->stack);
    gc_remove_ref (iterable);
}

static void iter_full (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * iterable;

    iterable = vector_pop (run_state->stack);;

    vector_push (run_state->stack, gc_add_ref (has_obj_iter_full (vm, iterable)));

    gc_remove_ref (iterable);
}

static void iter_next (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * iterable;

    iterable = vector_pop (run_state->stack);

    vector_push (run_state->stack, has_obj_iter_next (vm, iterable));

    gc_remove_ref (iterable);
}

static void jump (struct vm_state * vm, struct run_state * run_state, struct jump_inst * state) {
    run_state->pos = has_obj_get_label (run_state->obj, state->label) - 1;
}

static void jump_if_true (struct vm_state * vm, struct run_state * run_state, struct jump_if_true_inst * state) {
    struct has_obj * num_obj;
    int              num;

    num_obj = vector_pop (run_state->stack);
    num     = has_obj_to_cint (vm, num_obj);

    if (num != 0) {
        run_state->pos = has_obj_get_label (run_state->obj, state->label) - 1;
    }

    gc_remove_ref (num_obj);
}

static void jump_if_false (struct vm_state * vm, struct run_state * run_state, struct jump_if_false_inst * state) {
    struct has_obj * num_obj;
    int              num;

    num_obj = vector_pop (run_state->stack);
    num     = has_obj_to_cint (vm, num_obj);

    if (num == 0) {
        run_state->pos = has_obj_get_label (run_state->obj, state->label) - 1;
    }

    gc_remove_ref (num_obj);
}

static void list_decl (struct vm_state * vm, struct run_state * run_state, struct list_decl_inst * state) {
    struct vector_state * init;
    struct has_obj      * obj;

    init = vector_init ();
    for (int i = 0; i < state->count; i++) {
        obj = vector_pop (run_state->stack);
        vector_push (init, obj);
    }

    vector_push (run_state->stack, gc_add_ref (has_list_init (init)));
}

static void load_attrib (struct vm_state * vm, struct run_state * run_state, struct load_attrib_inst * state) {
    struct has_obj * target;
    struct has_obj * val;

    target = vector_pop (run_state->stack);
    val    = has_obj_get_attrib (target, state->attrib);

    if (has_obj_instanceof (vm, val, get_func_type ()) == HAS_TRUE) {
        val = has_bound_method_init (val, target);
        vector_push (run_state->stack, val);
    } else {
        vector_push (run_state->stack, val);
        gc_remove_ref (target);
    }

    gc_add_ref    (val);
}

static void load_global (struct vm_state * vm, struct run_state * run_state, struct load_global_inst * state) {
    struct has_obj * obj;

    obj = get_global (vm->stack_frame, state->symbol);

    if (obj == NULL) {
        printf ("Cannot load global %d\n", state->symbol);
        exit (EXIT_FAILURE);
    }

    vector_push (run_state->stack, gc_add_ref (obj));
}

static void load_id (struct vm_state * vm, struct run_state * run_state, struct load_id_inst * state) {
    struct has_obj * obj;

    obj = has_obj_get_attrib (run_state->obj, state->name);
    if (obj != NULL) {
        vector_push (run_state->stack, gc_add_ref (obj));
    } else {
        obj = has_obj_get_attrib (vm->mod, state->name);
        if (obj != NULL) {
            vector_push (run_state->stack, gc_add_ref (obj));
        } else {
            printf ("Cannot load id \"%s\"\n", state->name);
            exit   (EXIT_FAILURE);
        }
    }
}

static void load_local (struct vm_state * vm, struct run_state * run_state, struct load_local_inst * state) {
    struct has_obj * obj;

    obj = get_var (vm->stack_frame, state->symbol);

    if (obj == NULL) {
        obj = get_global (vm->stack_frame, state->symbol);
        if (obj == NULL) {
            printf ("Cannot load local %d\n", state->symbol);
            exit (EXIT_FAILURE);
        }
    }

    vector_push (run_state->stack, gc_add_ref (obj));
}

static void load_number (struct vm_state * vm, struct run_state * run_state, struct load_number_inst * state) {
    vector_push (run_state->stack, gc_add_ref (has_number_init (state->f)));
}

static void load_string (struct vm_state * vm, struct run_state * run_state, struct load_string_inst * state) {
    char * str;

    str = (char *)calloc (strlen (state->str) + 1, sizeof (char));
    memcpy (str, state->str, strlen (state->str) + 1);

    vector_push (run_state->stack, gc_add_ref (has_string_init (str)));
}

static void load_subscript (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * target;
    struct has_obj * index;

    target = vector_pop (run_state->stack);
    index  = vector_pop (run_state->stack);

    vector_push (run_state->stack, gc_add_ref (has_obj_index (vm, target, index)));

    gc_remove_ref (target);
    gc_remove_ref (index);
}

static void obj_decl (struct vm_state * vm, struct run_state * run_state, struct obj_decl_inst * state) {
    struct has_obj * obj;
    struct has_obj * vals [state->ids->length];

    for (int i = 0; i < state->ids->length; i++) {
        vals [i] = vector_pop (run_state->stack);
    }

    obj = has_obj_init (get_obj_type (), NULL, NULL);

    for (int i = 0; i < state->ids->length; i++) {
        has_obj_set_attrib (
            obj,
            vector_get (state->ids, i),
            vals [i]
        );
    }

    vector_push (run_state->stack, gc_add_ref (obj));

    for (int i = 0; i < state->ids->length; i++) {
        gc_remove_ref (vals [i]);
    }
}

static void obj_destructure_global (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_global_inst  * state) {
    struct has_obj  * target;
    struct has_list * list;
    char            * var;
    int               indice;

    target = vector_pop (run_state->stack);

    if (has_obj_instanceof (vm, target, get_list_type ()) == HAS_TRUE) {
        list = (struct has_list *)target->state;

        for (int i = 0; i < state->vars->length; i++) {
            indice = int_vector_get (state->indices, i);

            set_global (vm->stack_frame, indice, vector_get (list->vals, i));
        }
    } else {
        for (int i = 0; i < state->vars->length; i++) {
            indice = int_vector_get (state->indices, i);
            var    = vector_get     (state->vars,    i);

            set_global (vm->stack_frame, indice, has_obj_get_attrib (target, var));
        }
    }

    vector_push (run_state->stack, target);
}

static void obj_destructure_local (struct vm_state * vm, struct run_state * run_state, struct obj_destructure_local_inst   * state) {
    struct has_obj  * target;
    struct has_list * list;
    char            * var;
    int               indice;

    target = vector_pop (run_state->stack);

    if (has_obj_instanceof (vm, target, get_list_type ()) == HAS_TRUE) {
        list = (struct has_list *)target->state;

        for (int i = 0; i < state->vars->length; i++) {
            indice = int_vector_get (state->indices, i);

            set_var (vm->stack_frame, indice, vector_get (list->vals, i));
        }
    } else {
        for (int i = 0; i < state->vars->length; i++) {
            indice = int_vector_get (state->indices, i);
            var    = vector_get     (state->vars,    i);

            set_var (vm->stack_frame, indice, has_obj_get_attrib (target, var));
        }
    }

    vector_push (run_state->stack, target);
}

static void pop_ (struct vm_state * vm, struct run_state * run_state) {
    gc_remove_ref (vector_pop (run_state->stack));
}

static void pop_exception_handler (struct vm_state * vm, struct run_state * run_state) {
    vector_pop (vm->exception_handlers);
}

static void raise (struct vm_state * vm, struct run_state * run_state) {

}

static void self_reference (struct vm_state * vm, struct run_state * run_state) {
    vector_push (run_state->stack, gc_add_ref (run_state->self));
}

static void store_attrib (struct vm_state * vm, struct run_state * run_state, struct store_attrib_inst * state) {
    struct has_obj * target;
    struct has_obj * val;

    target = vector_pop (run_state->stack);
    val    = vector_pop (run_state->stack);

    has_obj_set_attrib (target, state->attrib, val);
    vector_push (run_state->stack, val);

    gc_remove_ref (target);
}

static void store_global (struct vm_state * vm, struct run_state * run_state, struct store_global_inst * state) {
    struct has_obj * val;

    val = vector_pop (run_state->stack);
    set_global (vm->stack_frame, state->symbol, val);

    vector_push (run_state->stack, val);
}

static void store_local (struct vm_state * vm, struct run_state * run_state, struct store_local_inst * state) {
    struct has_obj * val;

    val = vector_pop (run_state->stack);
    set_var (vm->stack_frame, state->symbol, val);

    vector_push (run_state->stack, val);
}

static void store_subscript (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * obj;
    struct has_obj * index;
    struct has_obj * val;

    obj   = vector_pop (run_state->stack);
    index = vector_pop (run_state->stack);
    val   = vector_pop (run_state->stack);

    has_obj_store_index (vm, obj, index, gc_add_ref (val));
    vector_push         (run_state->stack, val);

    gc_remove_ref (obj);
    gc_remove_ref (index);
}

static void super (struct vm_state * vm, struct run_state * run_state, struct super_inst * state) {

}

static void typeof (struct vm_state * vm, struct run_state * run_state) {
    struct has_obj * target;

    target = vector_pop (run_state->stack);

    vector_push (run_state->stack, gc_add_ref (typeof_has_obj (vm, target)));

    gc_remove_ref (target);
}

static void unary_op (struct vm_state * vm, struct run_state * run_state, struct unary_op_inst * state) {
    struct has_obj * target;

    target = vector_pop (run_state->stack);

    switch (state->type) {
        case logical_not_unary_op:
            vector_push (run_state->stack, target == HAS_TRUE ? HAS_FALSE : HAS_TRUE);
            break;
    }

    gc_remove_ref (target);
}

static void use_global (struct vm_state * vm, struct run_state * run_state, struct use_global_inst * state) {

}

static void use_local (struct vm_state * vm, struct run_state * run_state, struct use_local_inst * state) {

}
