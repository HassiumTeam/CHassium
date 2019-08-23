#ifndef _HAS_OBJ_H_
#define _HAS_OBJ_H_

#include <emit/inst.h>
#include <util/dict.h>
#include <util/int_dict.h>
#include <util/vector.h>

struct has_obj {
    struct dict_state     * attribs;
    struct has_obj        * type;
    struct vector_state   * instructions;
    struct int_dict_state * labels;
    int                     ref_count;

    void                  * state;
    void                 (* free_state) (void *);
};

#include <vm/gc.h>

struct has_obj * has_obj_init           (void * state, void (* free_state) (void *));
void             has_obj_free           (struct has_obj * obj);
void             has_obj_recursive_free (struct has_obj * obj);

struct has_obj * has_obj_get_attrib (struct has_obj * obj, char * name);
void             has_obj_set_attrib (struct has_obj * obj, char * name, struct has_obj * val);
void             has_obj_emit_label (struct has_obj * obj, int label);
void             has_obj_emit       (struct has_obj * obj, struct inst * inst);

#endif
