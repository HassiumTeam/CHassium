#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/vector.h>

#define SCOPE_SIZE 100

struct symbol_table_state {
    struct vector_state * scopes;
    int                   next_index;
};

struct symbol_table_state * symbol_table_init ();
void                        symbol_table_free (struct symbol_table_state * state);

void enter_scope     (struct symbol_table_state * state);
int  get_symbol      (struct symbol_table_state * state, char * id);
int  handle_symbol   (struct symbol_table_state * state, char * id);
int  has_symbol      (struct symbol_table_state * state, char * id);
int  in_global_scope (struct symbol_table_state * state);
void leave_scope     (struct symbol_table_state * state);
int  tmp_symbol      (struct symbol_table_state * state);

#endif
