#ifndef _SOURCEFILE_H_
#define _SOURCEFILE_H_

#include <ds/strbuf.h>
#include <ds/vec.h>
#include <util.h>

struct sourcefile {
  char *fileName;
  char *filePath;
  char *contents;
  struct vec *lines;
};

struct sourcepos {
  int row;
  int col;
  int refs;
  struct sourcefile *sourcefile;
};

struct sourcefile *sourcefile_new(char *);
void sourcefile_free(struct sourcefile *);

struct sourcepos *sourcepos_new(int, int, struct sourcefile *);
void sourcepos_to_strbuf(struct sourcepos *, struct strbuf *);

static inline struct sourcepos *sourcepos_inc_ref(struct sourcepos *sourcepos) {
  sourcepos->refs++;
  return sourcepos;
}

static inline struct sourcepos *sourcepos_dec_ref(struct sourcepos *sourcepos) {
  sourcepos->refs--;
  if (sourcepos->refs <= 0) {
    free(sourcepos);
  }
  return sourcepos;
}

#endif