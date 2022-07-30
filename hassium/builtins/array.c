#include <object.h>

static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);

struct obj *obj_array_new(struct vec *items) {
  struct obj *arr = obj_new(OBJ_ARRAY, items);
  obj_set_attrib(arr, "__iter__", obj_builtin_new(__iter__, arr));
  obj_set_attrib(arr, "length", obj_num_new(items->len));
  return arr;
}

int obj_array_len(struct obj *array) { return ((struct vec *)array->ctx)->len; }

static struct obj *__iter__(struct obj *arr, struct vm *vm, struct vec *args) {
  struct obj *iter = obj_iter_new(arr);
  obj_set_attrib(iter, "__iterfull__",
                 obj_builtin_new(__iter____iterfull__, iter));
  obj_set_attrib(iter, "__iternext__",
                 obj_builtin_new(__iter____iternext__, iter));
  return iter;
}

static struct obj *__iter____iterfull__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  struct vec *arr = iter->target->ctx;
  return bool_to_obj(iter->pos >= arr->len);
}

static struct obj *__iter____iternext__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  struct vec *arr = iter->target->ctx;
  return vec_get(arr, iter->pos++);
}