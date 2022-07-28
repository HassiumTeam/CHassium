#ifndef _intmap_H_
#define _intmap_H_

#include <stdlib.h>

#define DICT_SIZE 8

struct intmap {
  int *values;
  int len;
  int size;
};

struct intmap *intmap_new();
void intmap_free(struct intmap *);

void intmap_insert(struct intmap *, int, int);
int intmap_get(struct intmap *, int);

#endif