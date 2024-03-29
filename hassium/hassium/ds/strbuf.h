#ifndef _STRBUF_H_
#define _STRBUF_H_

#include <stdlib.h>
#include <string.h>

#define STRBUF_EXPAND_AT 8

struct strbuf {
  char *buf;
  int len;
  int size;
};

struct strbuf *strbuf_new();
char *strbuf_done(struct strbuf *);
void strbuf_append(struct strbuf *, char);
void strbuf_append_str(struct strbuf *, char *);

#endif