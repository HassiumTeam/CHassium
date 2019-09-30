#ifndef _LEXER_H_
#define _LEXER_H_

#include <ctype.h>
#include <lexer/token.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/string.h>
#include <util/vector.h>

struct lexer {
    char          * code;
    int             length;
    int             pos;
    struct vector * toks;
};

struct lexer * lexer_init_from_file (const char * fpath);
struct lexer * lexer_init_from_src  (char * code);
void           free_lexer           (struct lexer * lexer);

struct vector * lexer_run (struct lexer * lexer);

#endif
