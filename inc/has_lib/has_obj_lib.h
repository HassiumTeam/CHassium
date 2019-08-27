#ifndef _HAS_OBJ_LIB_H_
#define _HAS_OBJ_LIB_H_

#include <stdarg.h>
#include <stdio.h>
#include <has_lib/modules/default/string.h>
#include <has_lib/has_obj.h>

struct has_obj * has_obj_add        (struct vm_state * vm, struct has_obj * left, struct has_obj * right);
struct has_obj * has_obj_index      (struct vm_state * vm, struct has_obj * obj, struct has_obj * index);
struct has_obj * has_obj_invoke     (struct vm_state * vm, struct has_obj * obj, struct vector_state * args);
char           * has_obj_to_cstring (struct vm_state * vm, struct has_obj * obj);

struct vector_state * assemble_args (int arg_count, ...);

#endif
