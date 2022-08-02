#include <hassium.h>

struct hassium_ctx hassium_ctx = {
    .prog_path = NULL,
};

struct code_obj *compile_module(char *str) {
  struct vec *toks = lexer_tokenize(str);
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks);
  struct code_obj *module = compile_ast(ast);
  ast_node_free(ast);
  free_toks(toks);
  return module;
}

struct code_obj *compile_module_for_import(struct vec *from) {
  char rel_path[512];
  strcpy(rel_path, dirname(hassium_ctx.prog_path));
  strcat(rel_path, "/");
  strcat(rel_path, vec_get(from, 0));
  for (int i = 1; i < from->len; i++) {
    strcat(rel_path, "/");
    strcat(rel_path, vec_get(from, i));
  }
  strcat(rel_path, ".has");

  if (!file_exists(rel_path)) {
    printf("No such file %s\n", rel_path);
    exit(-1);
  }

  char *code = file_to_str(rel_path);
  struct code_obj *mod = compile_module(code);
  free(code);
  return mod;
}

void run_module(struct code_obj *mod) {
  struct vm *vm = vm_new();
  obj_dec_ref(vm_run(NULL, vm, mod));
  code_obj_free(mod);
  vm_free(vm);
}

void run_string(char *str) {
  struct code_obj *module = compile_module(str);
  run_module(module);
}