#ifndef _INST_H_
#define _INST_H_

#include <bin_op_type.h>
#include <stdlib.h>
#include <unary_op_type.h>

typedef enum {
    bin_op_inst, break_inst, build_closure_inst, build_exception_handler_inst, call_inst,
    continue_inst, compile_module_inst, import_inst, iter_inst, iter_next_inst, iter_full_inst,
    jump_inst, jump_if_true_inst, jump_if_false_inst, list_decl_inst, load_attrib_inst,
    load_id_inst, load_number_inst, load_string_inst, load_subscript_inst, obj_decl_inst,
    obj_destructure_global_inst, obj_destructure_local_inst, pop_inst,
    pop_instruction_handler_inst, push_inst, raise_inst, return_inst, self_reference_inst,
    store_attrib_inst, store_global_inst, store_local_inst, store_subscript_inst,
    super_inst, typeof_inst, unary_op_inst, use_global_inst, use_local_inst,
} inst_type_t;

struct inst {
    inst_type_t  type;
    void       * state;
};

struct bin_op_inst {
    bin_op_type_t type;
};

struct build_closure_inst {

};

struct build_exception_handler_inst {

};

struct call_inst {
    int arg_count;
};

struct compile_module_inst {
    char * file;
};

struct import_inst {
    char                * file;
    struct vector_state * name;
};

struct jump_inst {
    int label;
};

struct jump_if_true_inst {
    int label;
};

struct jump_if_false_inst {
    int label;
};

struct list_decl_inst {
    int count;
};

struct load_attrib_inst {
    char * attrib;
};

struct load_id_inst {
    int    index;
    char * name;
};

struct load_number_inst {
    float f;
};

struct load_string_inst {
    char * str;
};

struct obj_decl_inst {
    struct vector_state * ids;
};

struct obj_destructure_global_inst {
    struct vector_state * vars;
    struct vector_state * indices;
};

struct obj_destructure_local_inst {
    struct vector_state * vars;
    struct vector_state * indices;
};

struct store_attrib_inst {
    char * attrib;
};

struct store_global_inst {
    int symbol;
};

struct store_local_inst {
    int symbol;
};

struct unary_op_inst {
    unary_op_type_t type;
};

struct use_global_inst {
    struct vector_state * ids;
    struct vector_state * indices;
};

struct use_local_inst {
    struct vector_state * ids;
    struct vector_state * indices;
};

struct inst * bin_op_inst_init                 (bin_op_type_t type);
struct inst * break_inst_init                  ();
struct inst * call_inst_init                   (int arg_count);
struct inst * compile_module_inst_init         (char * mod);
struct inst * continue_inst_init               ();
struct inst * import_inst_init                 (char * file, struct vector_state * name);
struct inst * jump_inst_init                   (int label);
struct inst * jump_if_true_inst_init           (int label);
struct inst * jump_if_false_inst_init          (int label);
struct inst * list_decl_inst_init              (int count);
struct inst * load_attrib_inst_init            (char * attrib);
struct inst * load_id_inst_init                (int index, char * name);
struct inst * load_number_inst_init            (float f);
struct inst * load_string_inst_init            (char * str);
struct inst * obj_decl_inst_init               (struct vector_state * ids);
struct inst * obj_destructure_global_inst_init (struct vector_state * vars, struct vector_state * indices);
struct inst * obj_destructure_local_inst_init  (struct vector_state * vars, struct vector_state * indices);
struct inst * pop_inst_init                    ();
struct inst * store_attrib_inst_init           (char * attrib);
struct inst * store_global_inst_init           (int symbol);
struct inst * store_local_inst_init            (int symbol);
struct inst * unary_op_inst_init               (unary_op_type_t type);
struct inst * use_global_inst_init             (struct vector_state * ids, struct vector_state * indices);
struct inst * use_local_inst_init              (struct vector_state * ids, struct vector_state * indices);

void bin_op_inst_free                 (struct bin_op_inst * inst);
void break_inst_free                  ();
void call_inst_free                   (struct call_inst * inst);
void compile_module_inst_free         (struct compile_module_inst * inst);
void continue_inst_free               ();
void import_inst_free                 (struct import_inst * inst);
void jump_inst_free                   (struct jump_inst * inst);
void jump_if_true_inst_free           (struct jump_if_true_inst * inst);
void jump_if_false_inst_free          (struct jump_if_false_inst * inst);
void list_decl_inst_free              (struct list_decl_inst * inst);
void load_attrib_inst_free            (struct load_attrib_inst * inst);
void load_id_inst_free                (struct load_id_inst * inst);
void load_number_inst_free            (struct load_number_inst * inst);
void load_string_inst_free            (struct load_string_inst * inst);
void obj_decl_inst_free               (struct obj_decl_inst * inst);
void obj_destructure_global_inst_free (struct obj_destructure_global_inst * inst);
void obj_destructure_local_inst_free  (struct obj_destructure_local_inst * inst);
void pop_inst_free                    ();
void store_attrib_inst_free           (struct store_attrib_inst * inst);
void store_global_inst_free           (struct store_global_inst * inst);
void store_local_inst_free            (struct store_local_inst * inst);
void unary_op_inst_free               (struct unary_op_inst * inst);
void use_global_inst_free             (struct use_global_inst * inst);
void use_local_inst_free              (struct use_local_inst * inst);


#endif
