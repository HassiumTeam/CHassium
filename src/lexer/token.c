#include <lexer/token.h>

struct token * token_init () {
    return (struct token *)calloc (1, sizeof (struct token));
}

void token_free (struct token * token) {
    if (token->val) {
        free (token->val);
    }
    free (token);
}
