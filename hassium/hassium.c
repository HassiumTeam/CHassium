#include <hassium.h>

struct code_obj *compile_module(char *str) {
  struct vec *toks = lexer_tokenize(str);
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks);
  struct code_obj *module = compile_ast(ast);
  ast_node_free(ast);
  free_toks(toks);
  return module;
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