#ifndef _HAS_OBJ_H_
#define _HAS_OBJ_H_

#include <emit/inst.h>
#include <stdlib.h>
#include <util/vector.h>

struct has_obj {
    struct has_obj * type;
    struct vector  * instructions;
    int              ref_count;

    void           * state;
    void          (* free_state) (void *);
};

struct has_obj * has_obj_init (struct has_obj * type, void * state, void (* free_state) (void *));
void             free_has_obj (struct has_obj * obj);

void has_obj_emit (struct has_obj * obj, struct inst * inst);

#endif
