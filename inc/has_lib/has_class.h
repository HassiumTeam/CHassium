#ifndef _HAS_CLASS_H_
#define _HAS_CLASS_H_

#include <has_lib/modules/default/default_mod.h>
#include <has_lib/has_bound_func.h>
#include <has_lib/has_obj.h>
#include <has_lib/has_obj_lib.h>
#include <has_lib/has_type.h>
#include <stdlib.h>
#include <util/dict.h>
#include <util/vector.h>

struct has_class {
    char                * name;
    struct vector_state * extends;
    struct has_obj      * type;
};

struct has_obj * has_class_init (char * name, struct vector_state * extends);
void             has_class_free (void * state);

struct has_obj * get_class_type ();

struct has_obj * has_class_instantiate (struct vm_state * vm, struct has_obj * self);

#endif
