#include <hassium.h>

struct hassium_ctx hassium_ctx = {
    .prog_path = NULL,
};

struct code_obj *compile_module(struct sourcefile *sourcefile) {
  struct vec *toks = lexer_tokenize(sourcefile);
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks);
  struct code_obj *module = compile_ast(ast);
  ast_node_free(ast);
  free_toks(toks);
  return module;
}

struct code_obj *compile_module_for_import(struct vec *from) {
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
    printf("No such file %s\n", rel_path);
    exit(-1);
  }

  struct sourcefile *mod_file = sourcefile_new(rel_path);
  free(rel_path);
  struct code_obj *mod = compile_module(mod_file);
  sourcefile_free(mod_file);

  return mod;
}

void run_module(struct code_obj *mod) {
  struct vm *vm = vm_new();
  vec_push(vm->frames, stackframe_inc_ref(stackframe_new(mod->locals, NULL)));
  obj_dec_ref(vm_run(vm, mod, NULL));
  stackframe_dec_ref(vec_pop(vm->frames));
  code_obj_free(mod);
  vm_free(vm);
}

void run_file(char *fpath) {
  struct sourcefile *sourcefile = sourcefile_new(fpath);
  struct code_obj *module = compile_module(sourcefile);
  run_module(module);
  sourcefile_free(sourcefile);
  debug();
}