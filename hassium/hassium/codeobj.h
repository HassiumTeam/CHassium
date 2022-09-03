#ifndef _CODE_OBJ_H_
#define _CODE_OBJ_H_

#include <ds/intmap.h>
#include <ds/vec.h>
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
  int *pos;
  int locals;
  int caught_label;
  int refs;
};

struct code_obj *code_obj_new(char *);
void code_obj_free(struct code_obj *);

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

#endif