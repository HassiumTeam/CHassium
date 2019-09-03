#include <compiler.h>

struct has_obj * compile (char * file) {
    printf ("%s\n", file); // displays bin/test.has
    struct tokenizer_state * lexer = tokenizer_init(fopen (file, "r"));
    struct parser_state * parser = parser_init (lexer);
    struct ast_node * ast = parser_parse (parser);
    tokenizer_free (lexer);
    parser_free (parser);

    struct emit_state * emit = emit_init ();
    accept (emit, ast);
    ast_node_free (ast);
    struct has_obj * mod = emit_get_module (emit);
    emit_free (emit);

    return mod;
}
