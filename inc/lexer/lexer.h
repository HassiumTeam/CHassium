#ifndef _LEXER_H_
#define _LEXER_H_

#include <lexer/token.h>
#include <stdlib.h>
#include <string.h>
#include <util/string.h>
#include <util/vector.h>

struct vector * lexer_run (char * code);

#endif
