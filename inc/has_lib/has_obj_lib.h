#ifndef _HAS_OBJ_LIB_H_
#define _HAS_OBJ_LIB_H_

#include <stdio.h>
#include <has_lib/has_obj.h>

struct has_obj * has_obj_invoke     (struct has_obj * obj, struct vm_state * vm, struct has_obj * self, struct vector_state * args);

#endif
