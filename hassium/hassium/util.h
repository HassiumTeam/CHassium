#ifndef _UTIL_H_
#define _UTIL_H_

#include <ds/strbuf.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void *clone(void *, size_t);
char *clone_str(char *);
char *heap_str(int, ...);
char *file_to_str(char *);

#endif