#include <compiler.h>

void compile () {
    struct tokenizer_state * lexer = tokenizer_init(fopen ("bin/test.has", "rb"));
    struct parser_state * parser = parser_init (lexer);
    struct ast_node * ast = parser_parse (parser);
    tokenizer_free (lexer);
    parser_free (parser);


    printf ("Type: %d\n", ast->type);

    struct block_state * block = (struct block_state *)ast->state;
    struct vector_state * stmts = block->stmts;

    for (int i = 0; i < stmts->length; i++) {
        printf ("Subtype: %d\n", ((struct ast_node *)vector_get (stmts, i))->type);
    }

    struct emit_state * emit = emit_init ();
    accept (emit, ast);
    has_obj_free (emit_get_module (emit));
    ast_node_free (ast);
    emit_free (emit);
}
