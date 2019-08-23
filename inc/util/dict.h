#ifndef _DICT_H_
#define _DICT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/vector.h>

struct dict_state {
    struct vector_state * keys;
    struct vector_state * vals;
    int                   count;
};

struct dict_state * dict_init ();
void                dict_free (struct dict_state * state);

void * dict_get     (struct dict_state * state, char * key);
int    dict_has_key (struct dict_state * state, char * key);
void   dict_set     (struct dict_state * state, char * key, void * val);

#endif
