#ifndef _HAS_BOUND_FUNC_H_
#define _HAS_BOUND_FUNC_H_

#include <has_lib/has_func.h>
#include <has_lib/has_method.h>
#include <has_lib/has_obj_lib.h>
#include <has_lib/has_obj.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/vm.h>

struct has_bound_func {
    struct has_obj * func;
    struct has_obj * self;
};

struct has_obj * has_bound_func_init (struct has_obj * func, struct has_obj * self);
void             has_bound_func_free (void * state);

#endif
