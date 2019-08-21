#ifndef _INT_VECTOR_H_
#define _INT_VECTOR_H_

#include <stdlib.h>

#define INT_VECTOR_SIZE 100

struct int_vector_state {
    int * data;
    int   length;
    int   size;
};

struct int_vector_state * int_vector_init ();
void                      int_vector_free (struct int_vector_state * state);

int  int_vector_get  (struct int_vector_state *, int index);
int  int_vector_peek (struct int_vector_state *);
int  int_vector_pop  (struct int_vector_state *);
void int_vector_push (struct int_vector_state *, int i);

#endif
