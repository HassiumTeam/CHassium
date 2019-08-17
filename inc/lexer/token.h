#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <lexer/tok_type.h>
#include <stdlib.h>

struct token {
    char       * val;
    tok_type_t   type;
};

struct token * token_init ();
void           token_free (struct token * token);

#endif
