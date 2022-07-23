#include <emit.h>
#include <ds/vec.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <vm.h>

int main(int argc, char *argv[])
{
    struct vec *toks = lexer_tokenize("5");
    debug_toks(toks);
    struct ast_node *ast = parser_parse(toks);
    struct code_obj *module = compile_ast(ast);
    ast_node_free(ast);
    free_toks(toks);
    struct vm *vm = vm_new();
    code_obj_free(module);
    vm_free(vm);
}