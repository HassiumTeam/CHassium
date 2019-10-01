#ifndef _PARSER_H_
#define _PARSER_H_

#include <lexer/token.h>
#include <parser/ast.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/string.h>
#include <util/vector.h>

struct parser {
    struct vector * tokens;
    int             pos;
};

struct parser * parser_init (struct vector * tokens);
void            free_parser (struct parser * parser);

struct ast_node * parser_run  (struct parser * parser);

#endif
