#include <util.h>

char *heap_str(int argc, ...) {
  va_list args;
  va_start(args, argc);
  struct strbuf *strbuf = strbuf_new();
  for (int i = 0; i < argc; i++) strbuf_append(strbuf, va_arg(args, int));
  va_end(args);
  return strbuf_done(strbuf);
}

void *clone(void *ptr, size_t size) {
  void *new = malloc(size);
  memcpy(new, ptr, size);
  return new;
}

char *clone_str(char *str) {
  int len = strlen(str);
  char *new = (char *)calloc(len + 1, sizeof(char));
  memcpy(new, str, len + 1);
  return new;
}