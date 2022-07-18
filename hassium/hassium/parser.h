#ifndef _PARSER_H_
#define _PARSER_H_

#include <ast.h>
#include <ds/vec.h>
#include <lexer.h>
#include <stdbool.h>
#include <string.h>
#include <util.h>

struct ast_node *parser_parse(struct vec *);

#endif