#ifndef _HAS_NUMBER_H_
#define _HAS_NUMBER_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/vm.h>

struct has_number {
    float val;
};

struct has_obj * has_number_init (float f);
void             has_number_free (void * state);

#endif
