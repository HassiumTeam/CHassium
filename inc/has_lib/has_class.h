#ifndef _HAS_CLASS_H_
#define _HAS_CLASS_H_

#include <has_lib/has_obj.h>
#include <stdlib.h>
#include <util/vector.h>

struct has_class {
    char                * name;
    struct vector_state * extends;
};

struct has_obj * has_class_init (char * name, struct vector_state * extends);
void             has_class_free (void * state);

#endif
