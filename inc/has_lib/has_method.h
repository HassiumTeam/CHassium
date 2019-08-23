#ifndef _HAS_METHOD_H_
#define _HAS_METHOD_H_

#include <has_lib/has_obj.h>
#include <stdarg.h>
#include <stdlib.h>
#include <vm/vm.h>

struct has_method {
    struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, int arg_count, ...);
};

struct has_obj * has_method_init (struct has_obj * (* invoke) (struct vm_state * vm, struct has_obj * self, int arg_count, ...));
void             has_method_free (void * state);

#endif
