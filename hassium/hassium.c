#include <hassium.h>

struct hassium_ctx hassium_ctx = {
    .prog_path = NULL,
    .modules = NULL,
    .compiling = NULL,
    .module_values = NULL,
    .freed_code_objs = NULL,
};

struct code_obj *compile_module(struct sourcefile *sourcefile, struct vm *vm) {
  if (hassium_ctx.compiling == NULL) {
    hassium_ctx.compiling = vec_new();
  }
  vec_push(hassium_ctx.compiling, sourcefile->file_path);

  struct vec *toks = lexer_tokenize(sourcefile, vm);
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks, vm);
  struct code_obj *module = compile_ast(ast, sourcefile, vm);
  ast_node_free(ast);
  free_toks(toks);

  if (hassium_ctx.modules == NULL) {
    hassium_ctx.modules = code_obj_hashmap_new();
  }
  code_obj_hashmap_set(hassium_ctx.modules, sourcefile->file_path, module);

  vec_pop(hassium_ctx.compiling);
  return module;
}

struct code_obj *compile_module_for_import(struct code_obj *importer,
                                           struct vec *from, struct vm *vm) {
  char *rel_path = from_to_path(from);
  struct sourcefile *mod_file = sourcefile_new(rel_path);

  if (!file_exists(rel_path)) {
    vm_raise(vm, obj_file_not_found_error_new(obj_string_new(rel_path)));
  }

  if (is_compiling_module(mod_file->file_path)) {
    if (importer->waiting_on == NULL) {
      importer->waiting_on = vec_new();
    }

    vec_push(importer->waiting_on, clone_str(mod_file->file_path));
    sourcefile_free(mod_file);
    return NULL;
  }

  struct code_obj *mod;
  if ((mod = code_obj_hashmap_get(hassium_ctx.modules, mod_file->file_path))) {
    return mod;
  } else {
    mod = compile_module(mod_file, vm);
    code_obj_hashmap_set(hassium_ctx.modules, mod_file->file_path, mod);
    return mod;
  }
}

bool is_compiling_module(char *file_path) {
  for (int i = 0; i < hassium_ctx.compiling->len; ++i) {
    if (strcmp((char *)vec_get(hassium_ctx.compiling, i), file_path) == 0) {
      return true;
    }
  }
  return false;
}

struct obj *run_module(struct code_obj *code_obj, struct vm *vm,
                       bool resolve_imports) {
  struct obj *mod_func = obj_func_new(code_obj, NULL, NULL, NULL, false);

  vec_push(vm->frames,
           stackframe_inc_ref(stackframe_new(code_obj->locals, mod_func)));
  struct obj *ret = vm_run(vm, code_obj, NULL);
  stackframe_dec_ref(vec_pop(vm->frames));

  obj_free(mod_func);

  return ret;
}

void run_file(char *fpath) {
  struct vm *vm = vm_new();
  struct sourcefile *sourcefile = sourcefile_new(clone_str(fpath));
  struct code_obj *module = compile_module(sourcefile, vm);
  code_obj_resolve_modules(module, vm);

  // obj_dec_ref(run_module(module, vm, true));

  code_obj_dec_ref(module);
  vm_free(vm);

  debug();
}

void done() {
  hashmap_free(hassium_ctx.modules);
  vec_free(hassium_ctx.compiling);
  obj_hashmap_free(hassium_ctx.module_values);
  vec_free(hassium_ctx.freed_code_objs);
}

char *from_to_path(struct vec *from) {
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

  return rel_path;
}