#ifndef _UTIL_H_
#define _UTIL_H_

#include <ds/strbuf.h>
#include <stdarg.h>
#include <stdlib.h>

char *heap_str(int, ...);
char *clone_str(char *);

#endif