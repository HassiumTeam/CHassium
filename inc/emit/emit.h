#ifndef _EMIT_H_
#define _EMIT_H_

#include <emit/symbol_table.h>
#include <parser/ast.h>
#include <stdlib.h>
#include <util/string.h>
#include <util/vector.h>

struct emit {
    struct symbol_table * symbol_table;
};

struct emit * emit_init ();
void          free_emit (struct emit * emit);



#endif
