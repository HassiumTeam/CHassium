#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <ctype.h>
#include <lexer/token.h>
#include <src_loc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct tokenizer_state {
    char * code;
    int pos;
    int len;
    int col;
    int row;
};

struct tokenizer_state * tokenizer_init (FILE                   * f);
void                     tokenizer_free (struct tokenizer_state * state);

void tokenizer_next   (struct tokenizer_state * state, struct token * token);

int tokenizer_rewind  (struct tokenizer_state * state, int count);
int tokenizer_forward (struct tokenizer_state * state, int count);

int get_tokenizer_pos (struct tokenizer_state * state);
int set_tokenizer_pos (struct tokenizer_state * state, int pos);

#endif
