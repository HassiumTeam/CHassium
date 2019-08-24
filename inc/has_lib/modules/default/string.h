#ifndef _HAS_STRING_H_
#define _HAS_STRING_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdlib.h>
#include <string.h>
#include <util/vector.h>
#include <vm/vm.h>

struct has_string {
    char * val;
};

struct has_obj * has_string_init (char * val);
void             has_string_free (void * state);


#endif
