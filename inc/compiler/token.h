#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
  comma_tok, comp_tok, dot_tok, id_tok, op_tok, str_tok,
} tok_type_t;

struct token {
  tok_type_t   tok;
  char       * val;
};



#endif
