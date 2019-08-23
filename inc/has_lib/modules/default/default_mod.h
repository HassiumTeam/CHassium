#ifndef _DEFAULT_MOD_H_
#define _DEFAULT_MOD_H_

#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/vm.h>

struct has_obj * default_mod_init ();

struct has_obj * println (struct vm_state * vm, struct has_obj * self, int arg_count, ...);

#endif
