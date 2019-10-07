#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdlib.h>
#include <string.h>
#include <util/vector.h>

#define SCOPE_SIZE 20

struct scope {
    char ** ids;
    int   * vals;
    int     length;
    int     size;
};

struct symbol_table {
    struct vector * scopes;
    int             next;
};

struct symbol_table * symbol_table_init ();
void                  free_symbol_table (struct symbol_table * symbol_table);

void enter_scope     (struct symbol_table * table);
int  get_symbol      (struct symbol_table * table, char * id);
int  handle_symbol   (struct symbol_table * table, char * id);
int  has_symbol      (struct symbol_table * table, char * id);
int  in_global_scope (struct symbol_table * table);
void leave_scope     (struct symbol_table * table);
int  tmp_symbol      (struct symbol_table * table);

#endif
