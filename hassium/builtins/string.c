#include <builtins.h>

struct obj *obj_string_new(char *value) {
  struct obj *string = obj_new(OBJ_STRING, clone_str(value));
  return string;
}

char *obj_string_val(struct obj *string) { return string->ctx; }
