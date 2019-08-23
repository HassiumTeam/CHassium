#ifndef _HAS_METHOD_H_
#define _HAS_METHOD_H_

#include <has_lib/has_obj.h>
#include <stdlib.h>
#include <vm/vm.h>

struct has_method {
    struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
    struct has_obj * self;
};

struct has_obj * has_method_init (struct has_obj * self, struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, struct vector_state * args));
void             has_method_free (void * state);

#endif
