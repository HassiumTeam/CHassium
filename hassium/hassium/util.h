#ifndef _UTIL_H_
#define _UTIL_H_

#include <ds/strbuf.h>
#include <stdarg.h>
#include <stdlib.h>

char *heap_str(int, ...);
void *clone(void *, size_t size);
char *clone_str(char *);

#endif