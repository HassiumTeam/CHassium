#ifndef _VM_H_
#define _VM_H_

#include <has_lib/has_obj.h>
#include <vm/stack_frame.h>
#include <util/vector.h>

struct vm_state {
    struct has_obj      * mod;
    struct stack_frame  * stack_frame;
    struct vector_state * exception_handlers;
    struct vector_state * exception_returns;
};

struct vm_state * vm_init (struct has_obj  * mod);
void              vm_free (struct vm_state * state);

struct has_obj  * vm_run  (struct vm_state * vm, struct has_obj * obj, struct has_obj * self);

#endif
