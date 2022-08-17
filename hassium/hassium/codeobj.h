#ifndef _CODE_OBJ_H_
#define _CODE_OBJ_H_

#include <ds/intmap.h>
#include <ds/vec.h>
#include <vm.h>

struct code_obj {
  char *name;
  struct vec *instructions;
  struct vec *code_objs;
  struct vec *vecs;
  struct vec *strs;
  struct vec *params;
  struct intmap *labels;
  struct vec *break_labels;
  struct vec *cont_labels;
  struct vec *consts;
  int locals;
};

struct code_obj *code_obj_new(char *);
void code_obj_free(struct code_obj *);

#endif