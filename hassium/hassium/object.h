#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <ds/hashmap.h>
#include <sourcefile.h>
#include <stdlib.h>
#include <util.h>

struct vm {
  struct vec *frames;
  struct hashmap *globals;
  struct vec *handlers;
  struct vec *handler_returns;
};

struct stackframe {
  struct obj *invokee;
  struct obj **locals;
  int num_locals;
  int refs;
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

struct builtin_ops {
  builtin_func_t __add__;
  builtin_func_t __div__;
  builtin_func_t __eq__;
  builtin_func_t __greater__;
  builtin_func_t __iter__;
  builtin_func_t __lesser__;
  builtin_func_t __mod__;
  builtin_func_t __mul__;
  builtin_func_t __sub__;
};

struct obj {
  obj_ctx_type_t type;
  bool ref_immune;
  int refs;
  void *ctx;
  struct builtin_ops *ops;
  struct obj *obj_type;
  struct obj *parent;
  struct hashmap *attribs;
  void (*lazy_load_fn)(struct obj *);
  struct vec *weak_refs;
  struct sourcepos *sourcepos;
};

struct builtin_obj_ctx {
  builtin_func_t func;
  struct obj *self;
  char *name;
};

struct iter_obj_ctx {
  struct obj *target;
  int pos;
};

struct func_obj_ctx {
  struct code_obj *code_obj;
  struct vec *params;
  struct obj *self;
  struct stackframe *frame;
};

extern struct obj arg_mismatch_error_type_obj;
extern struct obj array_type_obj;
extern struct obj bool_type_obj;
extern struct obj builtin_type_obj;
extern struct obj error_type_obj;
extern struct obj func_type_obj;
extern struct obj iter_type_obj;
extern struct obj none_type_obj;
extern struct obj number_type_obj;
extern struct obj object_type_obj;
extern struct obj string_type_obj;
extern struct obj type_type_obj;
extern struct obj weakref_type_obj;

extern struct obj type_error_type_obj;

extern struct obj none_obj;
extern struct obj true_obj;
extern struct obj false_obj;

struct obj *obj_new(obj_ctx_type_t, void *, struct obj *);
void obj_free(struct obj *);

#define obj_inc_ref_fast(o) (++((o)->refs))
#define obj_dec_ref_fast(o) ((o)->refs > 1 ? --(o)->refs : obj_dec_ref(o))

static inline struct obj *obj_inc_ref(struct obj *obj) {
  ++obj->refs;
  return obj;
}

static inline struct obj *obj_dec_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  --obj->refs;
  if (obj->refs <= 0) obj_free(obj);
  return obj;
}

static inline struct obj *obj_down_ref(struct obj *obj) {
  if (obj == NULL || obj->ref_immune) return obj;
  obj->refs--;
  return obj;
}

struct obj *obj_enforce_type(struct obj *, struct obj *, struct vm *);
struct obj *obj_eq(struct obj *, struct obj *, struct vm *);
struct obj *obj_bin_op(bin_op_type_t, struct obj *, struct obj *, struct vm *);
struct obj *obj_index(struct obj *, struct obj *, struct vm *);
struct obj *obj_instantiate(struct obj *, struct vm *, struct vec *);
struct obj *obj_invoke(struct obj *, struct vm *, struct vec *);
struct obj *obj_invoke_attrib(struct obj *, char *, struct vm *, struct vec *);
bool obj_is(struct obj *, struct obj *);
struct obj *object_keys(struct obj *, struct vm *, struct vec *);
void obj_set_attrib(struct obj *, char *, struct obj *);
void obj_store_index(struct obj *, struct obj *, struct obj *, struct vm *);
struct obj *obj_to_string(struct obj *, struct vm *);

static inline struct stackframe *stackframe_inc_ref(struct stackframe *);
static inline struct stackframe *stackframe_dec_ref(struct stackframe *);

static inline struct stackframe *stackframe_new(int num_locals,
                                                struct obj *invokee) {
  struct stackframe *stackframe = malloc(sizeof(struct stackframe));
  stackframe->locals =
      num_locals > 0 ? calloc(num_locals, sizeof(struct obj *)) : NULL;
  stackframe->num_locals = num_locals;
  stackframe->refs = 0;
  stackframe->invokee = invokee;
  return stackframe;
}

static inline struct obj *stackframe_free(struct stackframe *stackframe) {
  for (int i = 0; i < stackframe->num_locals; i++) {
    if (stackframe->locals[i] != NULL) {
      obj_dec_ref(stackframe->locals[i]);
    }
  }
  if (stackframe->num_locals > 0) {
    free(stackframe->locals);
  }
  free(stackframe);
}

#define stackframe_get(s, i) ((s)->locals[(i)])
#define stackframe_set(s, i, v) ((s)->locals[(i)] = (v))

static inline struct stackframe *stackframe_inc_ref(
    struct stackframe *stackframe) {
  if (stackframe != NULL) {
    ++stackframe->refs;
  }
  return stackframe;
}

static inline struct stackframe *stackframe_dec_ref(
    struct stackframe *stackframe) {
  if (stackframe != NULL) {
    if (--stackframe->refs <= 0) {
      stackframe_free(stackframe);
      return NULL;
    }
  }
  return stackframe;
}

#define obj_hashmap_new() hashmap_create()

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

static inline bool obj_hashmap_has_get(struct hashmap *map, char *key,
                                       struct obj **out) {
  if (map == NULL) return false;

  if (hashmap_get(map, key, strlen(key), (uintptr_t *)out)) {
    return true;
  }
  return false;
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
