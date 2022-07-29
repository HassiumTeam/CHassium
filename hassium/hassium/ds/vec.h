#ifndef _VEC_H_
#define _VEC_H_

#include <stdbool.h>
#include <stdlib.h>

#define VEC_EXPAND_AT 8

struct vec {
  void **data;
  int len;
  int size;
};

struct vec *vec_new();
struct vec *vec_init(struct vec *);
void vec_free(struct vec *);
void vec_free_deep(struct vec *);

bool vec_has(struct vec *, void *);
void *vec_peek(struct vec *);
void *vec_pop(struct vec *);
void vec_push(struct vec *, void *);
void *vec_get(struct vec *, int);
void vec_set(struct vec *, int, void *);
void *vec_remove(struct vec *, void *);

#endif