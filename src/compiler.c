#include <inc/compiler.h>

void compile () {
    struct tokenizer_state * lexer = tokenizer_init(fopen ("bin/test.has", "r"));
    struct token * token = (struct token *)malloc (sizeof (struct token));

    do {
        tokenizer_next (lexer, token);

        printf ("Token type %d with val: '%s'\n", token->type, token->val);
    } while (token->type != eof);
}
