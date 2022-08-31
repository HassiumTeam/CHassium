#ifndef _VEC_H_
#define _VEC_H_

#include <stdbool.h>
#include <stdlib.h>

#define VEC_EXPAND_AT 16

struct vec {
  void **data;
  int len;
  int size;
  void *grow_with;
};

static inline struct vec *vec_init(struct vec *vec) {
  vec->data = malloc(VEC_EXPAND_AT * sizeof(void *));
  vec->len = 0;
  vec->size = VEC_EXPAND_AT;
  vec->grow_with = NULL;
  return vec;
}

static inline struct vec *vec_new() {
  struct vec *vec = malloc(sizeof(struct vec));
  return vec_init(vec);
}

static inline void expand(struct vec *vec) {
  vec->size *= 2;
  vec->data = realloc(vec->data, sizeof(void *) * vec->size);
}

static inline void vec_free(struct vec *vec) {
  free(vec->data);
  free(vec);
}

#define vec_get(v, i) ((v)->data[(i)])
#define vec_peek(v) ((v)->data[(v)->len - 1])
#define vec_pop(v) ((v)->data[--(v)->len])

static inline void vec_free_deep(struct vec *vec) {
  for (int i = 0; i < vec->len; ++i) free(vec_get(vec, i));
  vec_free(vec);
}

static inline void vec_push(struct vec *vec, void *val) {
  if (vec->len >= vec->size) expand(vec);
  vec->data[vec->len++] = val;
}

void vec_set(struct vec *, int, void *);
void *vec_remove(struct vec *, void *);

#endif