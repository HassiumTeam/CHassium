#include <ds/vec.h>
#include <emit.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <vm.h>

int main(int argc, char *argv[]) {
  struct vec *toks = lexer_tokenize(
      "func fib(n) { if (n == 0) return 0; else if (n == 1) return 1; else if "
      "(n == 2) return 1; else return fib(n - 1) + fib(n - 2); } "
      "println(fib(14));");
  // debug_toks(toks);
  struct ast_node *ast = parser_parse(toks);
  struct code_obj *module = compile_ast(ast);
  ast_node_free(ast);
  free_toks(toks);
  struct vm *vm = vm_new();
  obj_dec_ref(vm_run(vm, module));
  code_obj_free(module);
  vm_free(vm);
}
