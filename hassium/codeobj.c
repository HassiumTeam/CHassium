#include <codeobj.h>

struct code_obj *code_obj_new(char *name) {
  struct code_obj *code_obj =
      (struct code_obj *)calloc(1, sizeof(struct code_obj));
  code_obj->name = name;
  code_obj->parent = NULL;
  code_obj->params = NULL;
  code_obj->instructions = vec_new();
  code_obj->code_objs = vec_new();
  code_obj->vecs = vec_new();
  code_obj->strs = vec_new();
  code_obj->sourceposes = vec_new();
  code_obj->labels = intmap_new();
  code_obj->break_labels = vec_new();
  code_obj->cont_labels = vec_new();
  code_obj->consts = vec_new();
  code_obj->locals = 0;
  return code_obj;
}

void code_obj_free(struct code_obj *code_obj) {
  if (code_obj->name != NULL) {
    free(code_obj->name);
  }

  if (code_obj->params != NULL) {
    vec_free(code_obj->params);
  }

  for (int i = 0; i < code_obj->consts->len; ++i) {
    obj_dec_ref(vec_get(code_obj->consts, i));
  }

  for (int i = 0; i < code_obj->code_objs->len; ++i) {
    code_obj_free(vec_get(code_obj->code_objs, i));
  }
  vec_free(code_obj->code_objs);

  for (int i = 0; i < code_obj->vecs->len; ++i) {
    vec_free_deep((struct vec *)vec_get(code_obj->vecs, i));
  }
  vec_free(code_obj->vecs);

  for (int i = 0; i < code_obj->sourceposes->len; ++i) {
    sourcepos_dec_ref(vec_get(code_obj->sourceposes, i));
  }
  vec_free(code_obj->sourceposes);

  vec_free(code_obj->instructions);
  intmap_free(code_obj->labels);
  vec_free(code_obj->break_labels);
  vec_free(code_obj->cont_labels);
  vec_free_deep(code_obj->strs);
  vec_free(code_obj->consts);

  free(code_obj);
}