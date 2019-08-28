#ifndef _HAS_ITER_H_
#define _HAS_ITER_H_

#include <has_lib/modules/default/number.h>
#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/gc.h>
#include <vm/vm.h>

struct has_iter {
    struct vector_state * vals;
    int                   pos;
};

struct has_obj * has_iter_init (struct vector_state * vals);
void             has_iter_free (void * state);

#endif
