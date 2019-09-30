#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>

#define VECTOR_SIZE_INC 100

struct vector {
    void ** data;
    int     length;
    int     size;
};

struct vector * vector_init ();
void            free_vector (struct vector * vector);

void * vector_get  (struct vector * vector, int i);
void * vector_peek (struct vector * vector);
void * vector_pop  (struct vector * vector);
void   vector_push (struct vector * vector, void * ptr);
void   vector_set  (struct vector * vector, int i, void * ptr);

#endif
