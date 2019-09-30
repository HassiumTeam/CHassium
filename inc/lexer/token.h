#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdlib.h>

typedef enum {
    ASSIGN_TOK,  CBRACE_TOK, COLON_TOK,   COMMA_TOK,   COMP_TOK,    CPAREN_TOK,
    CSQUARE_TOK, DOT_TOK,    EOF_TOK,     ID_TOK,      NUMBERC_TOK, OBRACE_TOK,
    OP_TOK,      OPAREN_TOK, OSQUARE_TOK, SEMICOL_TOK, STRINGC_TOK, UNKNOWN_TOK,
} tok_type_t;

struct token {
    tok_type_t   type;
    char       * val;
};

struct token * token_init (tok_type_t type, char * val);
void           free_token (struct token * token);

#endif
