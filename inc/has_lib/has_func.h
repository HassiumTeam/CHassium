#ifndef _HAS_FUNC_H_
#define _HAS_FUNC_H_

#include <emit/inst.h>
#include <has_lib/has_obj.h>
#include <util/vector.h>

struct has_func {
    char * name;
};

struct has_obj * has_func_init (char * name);
void             has_func_free (void * state);

#endif
