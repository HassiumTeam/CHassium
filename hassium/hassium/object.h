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
  OBJ_ARRAY,
  OBJ_BOOL,
  OBJ_BUILTIN,
  OBJ_FUNC,
  OBJ_ITER,
  OBJ_NONE,
  OBJ_NUM,
  OBJ_STRING,
  OBJ_TYPE,
  OBJ_WEAKREF,
} obj_ctx_type_t;

struct obj {
  int refs;
  bool ref_immune;
  obj_ctx_type_t type;
  struct obj *obj_type;
  void *ctx;
  struct obj *parent;
  struct hashmap *attribs;
  struct vec *weak_refs;
};

struct builtin_obj_ctx {
  builtin_func_t func;
  struct obj *self;
};

struct iter_obj_ctx {
  struct obj *target;
  int pos;
};

struct func_obj_ctx {
  struct code_obj *code_obj;
  struct vec *params;
  struct obj *self;
};

struct num_obj_ctx {
  bool is_float;
  int val_int;
  float val_float;
};

extern struct obj array_type_obj;
extern struct obj bool_type_obj;
extern struct obj builtin_type_obj;
extern struct obj func_type_obj;
extern struct obj iter_type_obj;
extern struct obj none_type_obj;
extern struct obj number_type_obj;
extern struct obj object_type_obj;
extern struct obj string_type_obj;
extern struct obj type_type_obj;
extern struct obj weakref_type_obj;

extern struct obj none_obj;
extern struct obj true_obj;
extern struct obj false_obj;

struct obj *obj_new(obj_ctx_type_t, void *, struct obj *);
void obj_free(struct obj *);

static inline struct obj *obj_inc_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs++;
  return obj;
}

static inline struct obj *obj_dec_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs--;
  if (obj->refs <= 0) obj_free(obj);
  return obj;
}

static inline struct obj *obj_down_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs--;
  return obj;
}

struct obj *obj_eq(struct obj *, struct obj *, struct vm *);
struct obj *obj_bin_op(bin_op_type_t, struct obj *, struct obj *, struct vm *);
struct obj *obj_index(struct obj *, struct obj *, struct vm *);
struct obj *obj_instantiate(struct obj *, struct vm *, struct vec *);
struct obj *obj_invoke(struct obj *, struct vm *, struct vec *);
struct obj *obj_invoke_attrib(struct obj *, char *, struct vm *, struct vec *);
void obj_set_attrib(struct obj *, char *, struct obj *);
void obj_store_index(struct obj *, struct obj *, struct obj *, struct vm *);
struct obj *obj_to_string(struct obj *, struct vm *);

static inline struct hashmap *obj_hashmap_new() { return hashmap_create(); }
static inline struct obj *obj_hashmap_get(struct hashmap *map, char *key) {
  if (map == NULL) return &none_obj;

  struct obj *obj;
  if (hashmap_get(map, key, strlen(key), (uintptr_t *)&obj)) {
    return obj;
  }
  return &none_obj;
}
static inline bool obj_hashmap_has(struct hashmap *map, char *key) {
  if (map == NULL) return false;

  struct obj tmp;
  return hashmap_get(map, key, strlen(key), (uintptr_t *)&tmp);
}
static inline void obj_hashmap_set(struct hashmap *map, char *key,
                                   struct obj *val) {
  if (map == NULL) return;

  hashmap_set(map, key, strlen(key), (uintptr_t)val);
}
static void obj_hashmap_entry_free(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  obj_dec_ref((struct obj *)value);
}

static inline void obj_hashmap_free(struct hashmap *map) {
  hashmap_iterate(map, obj_hashmap_entry_free, NULL);
  hashmap_free(map);
}

#endif
