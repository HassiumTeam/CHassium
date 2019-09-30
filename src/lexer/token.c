#include <lexer/token.h>

struct token * token_init (tok_type_t type, char * val) {
    struct token * token;

    token       = (struct token *)calloc (1, sizeof (struct token));
    token->type = type;
    token->val  = val;

    return token;
}

void free_token (struct token * token) {
    if (token->val) {
        free (token->val);
    }

    free (token);
}
