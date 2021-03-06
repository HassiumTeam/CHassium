#ifndef _UTIL_STRING_H_
#define _UTIL_STRING_H_

#include <stdlib.h>
#include <string.h>

char * create_empty_str ();
char * append_str       (char * str, char * s);
char * append_char      (char * str, char   c);
char * string_copy      (char * str);
void * stack_to_heap    (void * ptr, size_t len);

#endif
