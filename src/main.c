#include <lexer/lexer.h>
#include <lexer/token.h>
#include <parser/ast.h>
#include <parser/parser.h>
#include <stdio.h>
#include <util/vector.h>

int main (int argc, char * argv []) {
    struct lexer * lexer = lexer_init_from_src ("\"Hello, World!\"");
    struct token * token;
    struct vector * tokens = lexer_run (lexer);


    for (int i = 0; i < tokens->length; i++) {
        token = vector_get (tokens, i);

        printf ("Type: %d, Value \"%s\"\n", token->type, token->val);
    }

    struct ast_node * ast;
    struct parser   * parser;

    parser = parser_init (tokens);
    ast    = parser_run  (parser);

    free_lexer (lexer);
    free_ast_node (ast);
    free_parser (parser);
}
