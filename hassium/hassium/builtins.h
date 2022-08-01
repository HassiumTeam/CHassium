#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#include <codeobj.h>
#include <ds/hashmap.h>
#include <ds/vec.h>
#include <object.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

typedef struct obj *(*builtin_func_t)(struct obj *, struct vm *, struct vec *);

struct hashmap *get_defaults();

struct obj *obj_array_new(struct vec *);
int obj_array_len(struct obj *);

void obj_bool_init(struct obj *);
void obj_bool_free(struct obj *);
bool obj_is_true(struct obj *, struct vm *);
bool obj_is_false(struct obj *, struct vm *);
struct obj *bool_to_obj(bool);

struct obj *obj_builtin_new(builtin_func_t, struct obj *);

struct obj *obj_iter_new(struct obj *);
struct obj *obj_iter_target(struct obj *);

struct obj *obj_func_new(struct code_obj *, struct vec *, struct obj *);

struct obj *obj_num_new(bool, int, float);
bool obj_num_is_float(struct obj *);
int obj_num_val(struct obj *);

struct obj *obj_string_new(char *);
char *obj_string_val(struct obj *);

struct obj *obj_weakref_new(struct obj *);

#endif