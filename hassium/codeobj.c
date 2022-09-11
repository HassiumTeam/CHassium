#include <codeobj.h>

struct code_obj *code_obj_new(char *name, bool is_resolved) {
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
  code_obj->sourcefile = NULL;
  code_obj->waiting_on = NULL;
  code_obj->imports = vec_new();
  code_obj->is_resolved = is_resolved;

  return code_obj;
}

void code_obj_free(struct code_obj *code_obj) {
  if (hassium_ctx.freed_code_objs == NULL) {
    hassium_ctx.freed_code_objs = vec_new();
  }

  if (code_obj == NULL) return;

  if (!vec_includes(hassium_ctx.freed_code_objs, code_obj)) {
    vec_push(hassium_ctx.freed_code_objs, code_obj);
  } else {
    return;
  }

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
    struct code_obj *c = vec_get(code_obj->code_objs, i);
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

  for (int i = 0; i < code_obj->imports->len; ++i) {
    obj_dec_ref(vec_get(code_obj->imports, i));
  }
  vec_free(code_obj->imports);

  vec_free(code_obj->sourceposes);
  vec_free(code_obj->instructions);
  intmap_free(code_obj->labels);
  vec_free(code_obj->break_labels);
  vec_free(code_obj->cont_labels);
  vec_free_deep(code_obj->strs);
  vec_free(code_obj->consts);

  if (code_obj->waiting_on != NULL) {
    vec_free_deep(code_obj->waiting_on);
  }

  if (code_obj->sourcefile != NULL) {
    sourcefile_free(code_obj->sourcefile);
  }

  free(code_obj);
}

void code_obj_resolve_modules(struct code_obj *code_obj, struct vm *vm) {
  if (code_obj == NULL || !code_obj->is_resolved) {
    return;
  }

  int resolved = 0;

  for (int i = 0; i < code_obj->code_objs->len; i++) {
    struct code_obj *mod = vec_get(code_obj->code_objs, i);

    if (mod == NULL) {
      char *file_path = vec_get(code_obj->waiting_on, resolved++);
      mod = code_obj_hashmap_get(hassium_ctx.modules, file_path);
      vec_set(code_obj->code_objs, i, mod);
    }
    code_obj->is_resolved = false;
    code_obj_resolve_modules(mod, vm);
  }

  if (code_obj->waiting_on != NULL) {
    vec_free_deep(code_obj->waiting_on);
    code_obj->waiting_on = NULL;
  }

  if (hassium_ctx.module_values == NULL) {
    hassium_ctx.module_values = obj_hashmap_new();
  }

  obj_hashmap_set(hassium_ctx.module_values, code_obj->sourcefile->file_path,
                  run_module(code_obj, vm, false));
}
