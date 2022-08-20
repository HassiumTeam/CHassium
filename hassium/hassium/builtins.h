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
#define bool_to_obj(b) ((b) ? &true_obj : &false_obj)

struct obj *obj_builtin_new(builtin_func_t, struct obj *);

struct obj *obj_iter_new(struct obj *);
struct obj *obj_iter_target(struct obj *);

struct obj *obj_func_new(struct code_obj *, struct vec *, struct obj *,
                         struct stackframe *, bool);

struct obj *obj_num_new(bool, int, float);
// int obj_num_val(struct obj *);
#define obj_num_val(o) ((uintptr_t)((o)->ctx))

struct obj *obj_string_new(char *);
char *obj_string_val(struct obj *);

struct obj *obj_weakref_new(struct obj *);

#endif