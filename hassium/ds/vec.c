#include <ds/vec.h>

static void expand(struct vec *);

void vec_set(struct vec *vec, int idx, void *val) {
  while (idx >= vec->size) expand(vec);
  if (vec->grow_with != NULL && idx > vec->len) {
    for (int i = vec->len; i < idx; ++i) {
      vec->data[i] = vec->grow_with;
    }
  }
  vec->data[idx] = val;
  if (idx >= vec->len) vec->len = idx + 1;
}

void *vec_remove(struct vec *vec, void *val) {
  for (int i = 0; i < vec->len; ++i)
    if (vec->data[i] == val) {
      for (int j = i + 1; j < vec->len; j++) vec->data[j - 1] = vec->data[j];
      return val;
    }
  return NULL;
}