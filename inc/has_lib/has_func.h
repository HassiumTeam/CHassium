#ifndef _HAS_FUNC_H_
#define _HAS_FUNC_H_

#include <emit/inst.h>
#include <has_lib/has_obj.h>
#include <has_lib/has_obj_lib.h>
#include <has_lib/has_type.h>
#include <stdlib.h>
#include <parser/ast.h>
#include <util/vector.h>
#include <vm/vm.h>

struct has_func {
    char                * name;
    struct vector_state * params;
    struct vector_state * enforced_ret;
    int                   is_closure;
};

struct has_obj * has_func_init (char * name, struct vector_state * params, struct vector_state * return_type);
void             has_func_free (void * state);

#endif
