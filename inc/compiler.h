#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <emit/emit.h>
#include <has_lib/has_obj.h>
#include <lexer/token.h>
#include <lexer/tokenizer.h>
#include <parser/ast.h>
#include <parser/node_type.h>
#include <parser/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/vector.h>

struct has_obj * compile ();

#endif
