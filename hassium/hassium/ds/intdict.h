#ifndef _intdict_H_
#define _intdict_H_

#include <stdlib.h>

#define DICT_SIZE 100

struct intdict
{
    int *values;
    int len;
    int size;
};

struct intdict *intdict_new();
void intdict_free(struct intdict *);

void intdict_insert(struct intdict *, int, int);
int intdict_get(struct intdict *, int);

#endif