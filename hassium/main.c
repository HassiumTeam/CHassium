#include <emit.h>
#include <ds/vec.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <vm.h>

int main(int argc, char *argv[])
{
    struct vec *toks = lexer_tokenize("func test (msg) { println (msg); } test('123'); test(45); ");
    debug_toks(toks);
    struct ast_node *ast = parser_parse(toks);
    struct code_obj *module = compile_ast(ast);
    ast_node_free(ast);
    free_toks(toks);
    struct vm *vm = vm_new();
    vm_run(vm, module);
    code_obj_free(module);
    vm_free(vm);
}