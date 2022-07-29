#include <object.h>

struct obj *obj_array_new(struct vec *items) {
  struct obj *obj = obj_new(OBJ_ARRAY, items);
  obj_setattr(obj, "length", obj_num_new(items->len));
  return obj;
}

int obj_array_len(struct obj *array) { return ((struct vec *)array->ctx)->len; }