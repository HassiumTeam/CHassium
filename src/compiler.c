#include <inc/compiler.h>

void compile () {
    struct tokenizer_state * lexer = tokenizer_init(fopen ("bin/test.has", "r"));
    struct token * token = (struct token *)malloc (sizeof (struct token));

    tokenizer_next (lexer, token);
    printf ("Type:%d Val:%s\n", token->type, token->val);
}
