#include <codeobj.h>

struct code_obj *code_obj_new(char *name) {
  struct code_obj *code_obj =
      (struct code_obj *)calloc(1, sizeof(struct code_obj));
  code_obj->name = name;
  code_obj->instructions = vec_new();
  code_obj->labels = intmap_new();
  code_obj->consts = vec_new();
  return code_obj;
}
