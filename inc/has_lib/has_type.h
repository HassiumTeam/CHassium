#ifndef _HAS_TYPE_H_
#define _HAS_TYPE_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/gc.h>
#include <vm/vm.h>

struct has_type {
    char * name;
};

struct has_obj * has_type_init (char * name);
void             has_type_free (void * state);

#endif
