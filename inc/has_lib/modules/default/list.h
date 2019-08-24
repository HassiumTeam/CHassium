#ifndef _HAS_LIST_H_
#define _HAS_LIST_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/gc.h>
#include <vm/vm.h>

struct has_list {
    struct vector_state * vals;
};

struct has_obj * has_list_init (struct vector_state * init);
void             has_list_free (void * state);


#endif
