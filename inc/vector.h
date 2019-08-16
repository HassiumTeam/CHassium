#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>

#define VECTOR_SIZE 100

struct vector_state {
    void ** data;
    int length;
    int size;
};

struct vector_state * vector_init ();
void                  vector_free (struct vector_state * state);

void    vector_add (struct vector_state * state, void * ptr);
void *  vector_get (struct vector_state * state, int index);
void    vector_set (struct vector_state * state, int index, void * val);

#endif
