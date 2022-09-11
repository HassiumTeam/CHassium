#ifndef _CODE_OBJ_H_
#define _CODE_OBJ_H_

#include <ds/hashmap.h>
#include <ds/intmap.h>
#include <ds/vec.h>
#include <sourcefile.h>
#include <vm.h>

struct code_obj {
  char *name;
  struct code_obj *parent;
  struct vec *instructions;
  struct vec *code_objs;
  struct vec *vecs;
  struct vec *strs;
  struct vec *sourceposes;
  struct vec *params;
  struct intmap *labels;
  struct vec *break_labels;
  struct vec *cont_labels;
  struct vec *consts;
  struct sourcefile *sourcefile;
  struct vec *waiting_on;
  struct vec *imports;
  bool is_resolved;
  int *pos;
  int locals;
  int caught_label;
  int refs;
};

#include <object.h>

struct code_obj *code_obj_new(char *, bool);
void code_obj_free(struct code_obj *);
void code_obj_resolve_modules(struct code_obj *, struct vm *);

static inline struct code_obj *code_obj_inc_ref(struct code_obj *code_obj) {
  code_obj->refs++;
  return code_obj;
}

static inline struct code_obj *code_obj_dec_ref(struct code_obj *code_obj) {
  if (--code_obj->refs <= 0) {
    code_obj_free(code_obj);
  }
  return code_obj;
}

#define code_obj_hashmap_new() hashmap_create()

static inline struct code_obj *code_obj_hashmap_get(struct hashmap *map,
                                                    char *key) {
  if (map == NULL) return NULL;

  struct code_obj *code_obj;
  if (hashmap_get(map, key, strlen(key), (uintptr_t *)&code_obj)) {
    return code_obj;
  }
  return NULL;
}

static inline void code_obj_hashmap_set(struct hashmap *map, char *key,
                                        struct code_obj *val) {
  if (map == NULL) return;

  hashmap_set(map, key, strlen(key), (uintptr_t)val);
}

#endif