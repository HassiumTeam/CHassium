#ifndef _INT_DICT_H_
#define _INT_DICT_H_

#include <stdlib.h>

#define INT_DICT_SIZE 100

struct int_dict_state {
    int * keys;
    int * vals;
    int   length;
    int   size;
};

struct int_dict_state * int_dict_init ();
void                    int_dict_free (struct int_dict_state * state);

int  int_dict_get (struct int_dict_state * state, int key);
void int_dict_set (struct int_dict_state * state, int key, int val);

#endif
