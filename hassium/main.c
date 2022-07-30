#include <ds/vec.h>
#include <emit.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <vm.h>

int main(int argc, char *argv[]) {
  struct vec *toks =
      lexer_tokenize("a = [1, 2, 3]; foreach (b in [4, 5, 6]) { println(b); }");
  debug_toks(toks);
  struct ast_node *ast = parser_parse(toks);
  struct code_obj *module = compile_ast(ast);
  ast_node_free(ast);
  free_toks(toks);
  struct vm *vm = vm_new();
  obj_dec_ref(vm_run(vm, module));
  code_obj_free(module);
  vm_free(vm);
}
