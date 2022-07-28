#include <object.h>

struct obj *array_obj_new(struct vec *items) {
  struct array_obj_ctx *ctx =
      (struct array_obj_ctx *)calloc(1, sizeof(struct array_obj_ctx));
  ctx->items = items;
  struct obj *obj = obj_new(NULL, OBJ_ARRAY, ctx);
  obj_setattrib(obj, "length", num_obj_new(items->len));
  return obj;
}

struct obj *array_obj_get(struct obj *obj, int index) {
  struct array_obj_ctx *array = obj->ctx;
  if (index >= array->items->len || index < 0) return none_obj();
  return vec_get(array->items, index);
}

struct obj *array_obj_set(struct obj *obj, int index, struct obj *val) {
  struct array_obj_ctx *array = obj->ctx;
  if (index < 0) {
    printf("Index can't be less than 0\n");
    exit(-1);
  }
  vec_set(array->items, index, val);
  return val;
}