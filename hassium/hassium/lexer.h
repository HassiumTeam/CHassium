#ifndef _LEXER_H_
#define _LEXER_H_

#include <ctype.h>
#include <ds/strbuf.h>
#include <ds/vec.h>
#include <object.h>
#include <sourcefile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

typedef enum {
  TOK_ASSIGN,
  TOK_CBRACE,
  TOK_CSQUARE,
  TOK_CPAREN,
  TOK_COLON,
  TOK_COMMA,
  TOK_DOT,
  TOK_ID,
  TOK_OBRACE,
  TOK_OP,
  TOK_OPAREN,
  TOK_OSQUARE,
  TOK_NUM,
  TOK_SEMICOLON,
  TOK_STRING,
} toktype_t;

struct tok {
  toktype_t type;
  char *val;
  struct sourcepos *sourcepos;
};

struct tok *tok_new(toktype_t, char *, struct sourcepos *);
void tok_free(struct tok *);

struct vec *lexer_tokenize(struct sourcefile *, struct vm *);
void free_toks(struct vec *);
void debug_toks(struct vec *);

#endif