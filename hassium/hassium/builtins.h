#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#include <codeobj.h>
#include <ds/hashmap.h>
#include <ds/vec.h>
#include <object.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

typedef struct obj *(*builtin_func_t)(struct obj *, struct vm *, struct vec *);

struct hashmap *get_defaults();

struct obj *obj_builtin_new(builtin_func_t, struct obj *);

struct obj *obj_func_new(struct code_obj *, struct vec *);

struct obj *obj_num_new(float);
float obj_num_val(struct obj *);

struct obj *obj_string_new(char *);
char *obj_string_val(struct obj *);

struct obj *obj_weakref_new(struct obj *);

#endif