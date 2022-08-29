#include <sourcefile.h>

struct sourcefile *sourcefile_new(char *filePath) {
  struct sourcefile *sourcefile = malloc(sizeof(struct sourcefile));
  sourcefile->filePath = filePath;
  sourcefile->fileName = clone_str(filePath);
  sourcefile->contents = file_to_str(filePath);
  sourcefile->lines = vec_new();

  struct strbuf *line = strbuf_new();
  int len = strlen(sourcefile->contents);
  for (int i = 0; i < len; ++i) {
    char c = sourcefile->contents[i];
    if (c == '\n' || i + 1 == len) {
      vec_push(sourcefile->lines, strbuf_done(line));
      line = strbuf_new();
    } else {
      strbuf_append(line, c);
    }
  }
  vec_push(sourcefile->lines, strbuf_done(line));

  return sourcefile;
}

void sourcefile_free(struct sourcefile *sourcefile) {
  free(sourcefile->fileName);
  free(sourcefile->contents);
  vec_free_deep(sourcefile->lines);
  free(sourcefile);
}

struct sourcepos *sourcepos_new(int row, int col,
                                struct sourcefile *sourcefile) {
  struct sourcepos *sourcepos = malloc(sizeof(struct sourcepos));
  sourcepos->row = row;
  sourcepos->col = col;
  sourcepos->refs = 0;
  sourcepos->sourcefile = sourcefile;

  return sourcepos;
}

void sourcepos_to_strbuf(struct sourcepos *sourcepos, struct strbuf *strbuf) {
  strbuf_append_str(strbuf, sourcepos->sourcefile->fileName);
  strbuf_append(strbuf, ':');
  char row[0xF];
  char col[0xF];
  sprintf(row, "%d", sourcepos->row + 1);
  sprintf(col, "%d", sourcepos->col + 1);
  strbuf_append_str(strbuf, row);
  strbuf_append(strbuf, ':');
  strbuf_append_str(strbuf, col);
}