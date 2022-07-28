#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <ds/hashmap.h>
#include <stdlib.h>
#include <util.h>

struct vm {
  struct vec *frames;
};

#include <vm.h>

typedef struct obj *(*builtin_func_t)(struct obj *, struct vm *, struct vec *);

typedef enum {
  OBJ_ANON,
  OBJ_BOOL,
  OBJ_BUILTIN,
  OBJ_CLASS,
  OBJ_FUNC,
  OBJ_NONE,
  OBJ_NUM,
  OBJ_STRING,
  OBJ_WEAKREF,
} obj_ctx_type_t;

struct obj {
  long id;
  int refs;
  bool ref_immune;
  obj_ctx_type_t type;
  void *ctx;
  struct obj *parent;
  struct hashmap *attribs;
  struct vec *weak_refs;
};

struct obj_hashmap_entry {
  char *name;
  struct obj *obj;
};

struct builtin_obj_ctx {
  builtin_func_t func;
  struct obj *self;
};

struct func_obj_ctx {
  struct code_obj *code_obj;
  struct vec *params;
};

struct num_obj_ctx {
  float value;
};

extern struct obj none_obj;
extern struct obj true_obj;
extern struct obj false_obj;

struct obj *obj_new(obj_ctx_type_t, void *);
void obj_free(struct obj *);

struct obj *obj_inc_ref(struct obj *);
struct obj *obj_dec_ref(struct obj *);

struct obj *obj_bin_op(bin_op_type_t, struct obj *, struct obj *, struct vm *);
struct obj *obj_invoke(struct obj *, struct vm *, struct vec *);
void obj_setattr(struct obj *, char *, struct obj *);
struct obj *obj_to_string(struct obj *, struct vm *);

struct hashmap *obj_hashmap_new();
struct obj *obj_hashmap_get(struct hashmap *, char *);
void obj_hashmap_set(struct hashmap *, char *, struct obj *);
void obj_hashmap_free(struct hashmap *);

#endif
