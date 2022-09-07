#include <hassium.h>

struct hassium_ctx hassium_ctx = {
    .prog_path = NULL,
};

struct code_obj *compile_module(struct sourcefile *sourcefile, struct vm *vm) {
  struct vec *toks = lexer_tokenize(sourcefile);
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks, vm);
  struct code_obj *module = compile_ast(ast, vm);
  ast_node_free(ast);
  free_toks(toks);
  return module;
}

struct code_obj *compile_module_for_import(struct vec *from, struct vm *vm) {
  char *rel_path = calloc(512, sizeof(char));
  char *prog_path = clone_str(hassium_ctx.prog_path);

  strcpy(rel_path, dirname(prog_path));
  free(prog_path);
  strcat(rel_path, "/");
  strcat(rel_path, vec_get(from, 0));
  for (int i = 1; i < from->len; ++i) {
    strcat(rel_path, "/");
    strcat(rel_path, vec_get(from, i));
  }
  strcat(rel_path, ".has");

  if (!file_exists(rel_path)) {
    vm_raise(vm, obj_file_not_found_error_new(obj_string_new(rel_path)));
  }

  struct sourcefile *mod_file = sourcefile_new(rel_path);
  free(rel_path);
  struct code_obj *mod = compile_module(mod_file, vm);
  sourcefile_free(mod_file);

  return mod;
}

void run_module(struct code_obj *code_obj, struct vm *vm) {
  struct obj *mod_func = obj_func_new(code_obj, NULL, NULL, NULL, false);

  vec_push(vm->frames,
           stackframe_inc_ref(stackframe_new(code_obj->locals, mod_func)));
  obj_dec_ref(vm_run(vm, code_obj, NULL));
  stackframe_dec_ref(vec_pop(vm->frames));
  obj_free(mod_func);
  code_obj_dec_ref(code_obj);
  vm_free(vm);
}

void run_file(char *fpath) {
  struct vm *vm = vm_new();
  struct sourcefile *sourcefile = sourcefile_new(fpath);
  struct code_obj *module = compile_module(sourcefile, vm);
  run_module(module, vm);
  sourcefile_free(sourcefile);
  debug();
}