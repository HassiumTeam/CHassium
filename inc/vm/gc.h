#ifndef _GC_H_
#define _GC_H_

#include <has_lib/has_obj.h>
#include <util/vector.h>

void gc_init ();
void gc_free ();

void gc_add        (struct has_obj * obj);
void gc_add_ref    (struct has_obj * obj);
void gc_remove_ref (struct has_obj * obj);
void gc_run        ();

#endif
