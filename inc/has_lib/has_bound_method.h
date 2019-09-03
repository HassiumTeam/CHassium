#ifndef _HAS_BOUND_METHOD_H_
#define _HAS_BOUND_METHOD_H_

#include <has_lib/has_method.h>

struct has_bound_method {
    struct has_obj * method;
    struct has_obj * parent;
};

struct has_obj * has_bound_method_init (struct has_obj * method, struct has_obj * parent);
void             has_bound_method_free (void * state);

#endif
