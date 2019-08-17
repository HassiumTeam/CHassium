#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdlib.h>

typedef enum {
     assign, cbrace, colon, comma, comp, cparen, csquare, dot, eof, id,
     numberc, obrace, op, oparen, osquare, semicol, stringc, unknown,
} tok_type_t;

struct token {
    char       * val;
    tok_type_t   type;
};

struct token * token_init ();
void           token_free (struct token * token);

#endif
