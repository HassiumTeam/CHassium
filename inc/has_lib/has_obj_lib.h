#ifndef _HAS_OBJ_LIB_H_
#define _HAS_OBJ_LIB_H_

#include <stdarg.h>
#include <stdio.h>
#include <has_lib/modules/default/string.h>
#include <has_lib/has_obj.h>

struct has_obj * has_obj_invoke     (struct has_obj * obj, struct vm_state * vm, struct vector_state * args);
struct has_obj * has_obj_add        (struct vm_state * vm, struct has_obj * left, struct has_obj * right);
char           * has_obj_to_cstring (struct has_obj * obj, struct vm_state * vm);

struct vector_state * assemble_args (int arg_count, ...);

#endif
