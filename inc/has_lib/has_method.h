#ifndef _HAS_METHOD_H_
#define _HAS_METHOD_H_

#include <has_lib/has_func.h>
#include <has_lib/has_obj.h>
#include <has_lib/has_type.h>
#include <stdlib.h>
#include <vm/vm.h>

struct has_method {
    struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, struct vector_state * args);
    struct has_obj * self;
};

struct has_obj * has_method_init (struct has_obj * self, struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, struct vector_state * args), struct has_obj * type);
void             has_method_free (void * state);

struct has_obj * get_func_type   ();
struct has_obj * get_method_type ();

#endif
