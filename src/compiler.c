#include <compiler.h>

void compile () {
    struct tokenizer_state * lexer = tokenizer_init(fopen ("bin/test.has", "r"));
    struct token * token = (struct token *)malloc (sizeof (struct token));

    do {
        tokenizer_next (lexer, token);

//        printf ("Token type %d with val: '%s'\n", token->type, token->val);
    } while (token->type != eof);

    tokenizer_rewind (lexer, lexer->pos);

    struct parser_state * parser = parser_init (lexer);
    struct ast_node * ast = parser_parse (parser);

    printf ("Type: %d\n", ast->type);

    struct block_state * block = (struct block_state *)ast->state;
    struct vector_state * stmts = block->stmts;

    for (int i = 0; i < stmts->length; i++) {
        printf ("Subtype: %d\n", ((struct ast_node *)vector_get (stmts, i))->type);
    }

    struct emit_state * emit = emit_init ();
    accept (emit, ast);
    ast_node_free (ast);
    emit_free (emit);
}
