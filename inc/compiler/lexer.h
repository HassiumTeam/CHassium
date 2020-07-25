#ifndef _LEXER_H_
#define _LEXER_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include <compiler/token.h>
#include <util/vector.h>

struct lexer {
  vector_t * toks;
  char     * src;
  int        pos;
  int        len;
};

struct lexer * lexer_init (char * src);
void           lexer_free (struct lexer * l);

void           lexer_run  ();

#endif
