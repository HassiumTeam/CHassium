#include <parser/parser.h>

struct parser_state * parser_init (struct tokenizer_state * lexer) {
    struct parser_state * state = (struct parser_state *)malloc (sizeof (struct parser_state));

    state->lexer = lexer;
    state->token = (struct token *)malloc (sizeof (struct token));

    return state;
}

void parser_free (struct parser_state * state) {
    free (state->token);
    free (state);
}

struct ast_node * parse (struct parser_state * state) {
    
}
