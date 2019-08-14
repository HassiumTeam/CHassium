#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <inc/lexer/tok_type.h>

struct token {
    char * val;
    tok_type_t type;
};

#endif
