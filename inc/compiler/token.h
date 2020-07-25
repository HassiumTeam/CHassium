#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdlib.h>

typedef enum {
  comma_tok, comp_tok, dot_tok, id_tok, op_tok, str_tok,
} tok_type_t;

struct token {
  tok_type_t   type;
  char       * str;
};

struct token * token_init (tok_type_t type, char * str);
void           token_free (struct token * t);

#endif
