#include <codeobj.h>

struct code_obj *code_obj_new(char *name) {
  struct code_obj *code_obj =
      (struct code_obj *)calloc(1, sizeof(struct code_obj));
  code_obj->name = name;
  code_obj->params = NULL;
  code_obj->instructions = vec_new();
  code_obj->code_objs = vec_new();
  code_obj->vecs = vec_new();
  code_obj->strs = vec_new();
  code_obj->import_attribs = vec_new();
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
  vec_free(code_obj->code_objs);
  vec_free(code_obj->vecs);
  vec_free(code_obj->strs);
  vec_free(code_obj->instructions);
  intmap_free(code_obj->labels);
  vec_free(code_obj->break_labels);
  vec_free(code_obj->cont_labels);
  for (int i = 0; i < code_obj->consts->len; i++) {
    obj_dec_ref(vec_get(code_obj->consts, i));
  }
  vec_free(code_obj->consts);
  free(code_obj);
}

// static void vm_inst_free(struct vm_inst *inst) {
//   // printf("freeing inst %d\n", inst->type);
//   return;
//   switch (inst->type) {
//     case INST_BUILD_CLASS:
//       code_obj_free(((struct build_class_inst *)inst->inner)->code_obj);
//       break;
//     case INST_BUILD_FUNC: {
//       struct build_func_inst *build_func = inst->inner;
//       code_obj_free(build_func->code_obj);
//       vec_free(build_func->params);
//     } break;
//     case INST_BUILD_OBJ:
//       vec_free_deep(((struct build_obj_inst *)inst->inner)->keys);
//       break;
//     case INST_IMPORT: {
//       struct import_inst *import = inst->inner;
//       vec_free_deep(import->imports);
//       code_obj_free(import->mod);
//     } break;
//     case INST_ITER_NEXT:
//       free(((struct iter_next_inst *)inst->inner)->id);
//       break;
//     case INST_LOAD_ATTRIB:
//       free(((struct load_attrib_inst *)inst->inner)->attrib);
//       break;
//     case INST_LOAD_ID:
//       free(((struct load_id_inst *)inst->inner)->id);
//       break;
//     case INST_STORE_ATTRIB:
//       free(((struct store_attrib_inst *)inst->inner)->attrib);
//       break;
//     case INST_STORE_ID:
//       free(((struct store_id_inst *)inst->inner)->id);
//       break;
//   }
//   if (inst->inner != NULL) free(inst->inner);
//   int type = inst->type;
//   free(inst);
// }