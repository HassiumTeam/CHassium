#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_BUF_INC_SIZE 0xFF

typedef struct {
  void ** data;
  int     len;
  int     size;
} vector_t;

vector_t * vector_init      ();
void       vector_free      (vector_t * v);
void       vector_deep_free (vector_t * v, void (*free_fn)(void *));

void * vector_pop  (vector_t * v);
void   vector_push (vector_t * v, void * ptr);
void * vector_get  (vector_t * v, int i);
void   vector_set  (vector_t * v, int i, void * ptr);

#endif
