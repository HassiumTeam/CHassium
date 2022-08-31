#include <util.h>

void *clone(void *ptr, size_t size) {
  void *new = malloc(size);
  memcpy(new, ptr, size);
  return new;
}

char *clone_str(char *str) {
  if (str == NULL) return NULL;
  int len = strlen(str);
  char *new = (char *)calloc(len + 1, sizeof(char));
  memcpy(new, str, len + 1);
  return new;
}

char *heap_str(int argc, ...) {
  va_list args;
  va_start(args, argc);
  struct strbuf *strbuf = strbuf_new();
  for (int i = 0; i < argc; ++i) strbuf_append(strbuf, va_arg(args, int));
  va_end(args);
  return strbuf_done(strbuf);
}

bool file_exists(char *filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

char *file_to_str(char *fpath) {
  char *str = NULL;
  FILE *file = fopen(fpath, "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    str = malloc(len + 1);
    fread(str, 1, len, file);
    fclose(file);
    str[len] = 0;
  } else {
    printf("No such file %s\n", fpath);
  }

  return str;
}