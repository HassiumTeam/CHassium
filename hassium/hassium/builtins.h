#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#include <ds/vec.h>
#include <object.h>
#include <stdlib.h>

typedef struct obj *(*builtin_func_t)(struct obj *, struct vm *, struct vec *);

struct obj *obj_num_new(float);
float obj_num_val(struct obj *);

struct obj *obj_builtin_new(builtin_func_t, struct obj *);

struct obj *obj_weakref_new(struct obj *);
struct obj *obj_weakref_val(struct obj *);

#endif