#ifndef _UTIL_H_
#define _UTIL_H_

#include <ds/strbuf.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void *clone(void *, size_t);
char *clone_str(char *);
char *heap_str(int, ...);
bool file_exists(char *);
char *file_to_str(char *);

#endif