#ifndef _DEFAULT_MOD_H_
#define _DEFAULT_MOD_H_

#include <has_lib/modules/default/iter.h>
#include <has_lib/modules/default/list.h>
#include <has_lib/modules/default/number.h>
#include <has_lib/modules/default/string.h>
#include <has_lib/has_class.h>
#include <has_lib/has_method.h>
#include <has_lib/has_obj.h>
#include <has_lib/has_obj_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>
#include <vm/vm.h>

#define HAS_TRUE  has_obj_get_attrib (get_default_mod (), "true")
#define HAS_FALSE has_obj_get_attrib (get_default_mod (), "false")
#define HAS_NULL  has_obj_get_attrib (get_default_mod (), "null")

struct has_obj * get_default_mod  ();

#endif
