#ifndef _HAS_FUNC_H_
#define _HAS_FUNC_H_

#include <emit/inst.h>
#include <has_lib/has_obj.h>
#include <util/dict.h>
#include <util/int_dict.h>
#include <util/vector.h>

struct has_func {
    struct vector_state   * instructions;
    struct int_dict_state * labels;
};

struct has_obj * has_func_init ();
void             has_func_free (struct has_obj * obj);

#endif
