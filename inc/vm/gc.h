#ifndef _GC_H_
#define _GC_H_

#include <has_lib/has_obj.h>
#include <util/vector.h>

struct has_obj      * gc_add_ref     (struct has_obj * obj);
struct has_obj      * gc_remove_ref  (struct has_obj * obj);
struct vector_state * gc_remove_vect (struct vector_state * obj);

#endif
