#ifndef _PARSER_H_
#define _PARSER_H_

#include <lexer/tok_type.h>
#include <lexer/token.h>
#include <lexer/tokenizer.h>
#include <parser/ast.h>
#include <parser/node_type.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

struct parser_state {
    struct tokenizer_state * lexer;
    struct token           * token;
};

struct parser_state * parser_init (struct tokenizer_state * lexer);
void                  parser_free (struct parser_state    * state);

struct ast_node * parser_parse (struct parser_state * state);

#endif
