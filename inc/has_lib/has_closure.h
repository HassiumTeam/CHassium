#ifndef _HAS_CLOSURE_H_
#define _HAS_CLOSURE_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <has_lib/has_type.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/stack_frame.h>
#include <vm/vm.h>

struct has_closure {
    struct has_obj * func;
    struct frame   * frame;
};

struct has_obj * has_closure_init (struct has_obj * func, struct frame * frame);
void             has_closure_free (void * state);

#endif
