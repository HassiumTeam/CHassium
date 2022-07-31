#include <object.h>

static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *length(struct obj *, struct vm *, struct vec *);
static struct obj *peek(struct obj *, struct vm *, struct vec *);
static struct obj *pop(struct obj *, struct vm *, struct vec *);
static struct obj *push(struct obj *, struct vm *, struct vec *);

struct obj *obj_array_new(struct vec *items) {
  struct obj *arr = obj_new(OBJ_ARRAY, items, &array_type_obj);
  for (int i = 0; i < items->len; i++) obj_inc_ref(vec_get(items, i));

  obj_set_attrib(arr, "__iter__", obj_builtin_new(__iter__, arr));
  obj_set_attrib(arr, "length", obj_builtin_new(length, arr));
  obj_set_attrib(arr, "peek", obj_builtin_new(peek, arr));
  obj_set_attrib(arr, "pop", obj_builtin_new(pop, arr));
  obj_set_attrib(arr, "push", obj_builtin_new(push, arr));
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

static struct obj *length(struct obj *arr, struct vm *vm, struct vec *args) {
  return obj_num_new(false, obj_array_len(arr), 0);
}

static struct obj *peek(struct obj *arr, struct vm *vm, struct vec *args) {
  if (obj_array_len(arr) == 0) return &none_obj;
  return vec_peek(arr->ctx);
}

static struct obj *pop(struct obj *arr, struct vm *vm, struct vec *args) {
  if (obj_array_len(arr) == 0) return &none_obj;
  return obj_down_ref(vec_pop(arr->ctx));
}

static struct obj *push(struct obj *arr, struct vm *vm, struct vec *args) {
  for (int i = 0; i < args->len; i++) {
    vec_push(arr->ctx, obj_inc_ref(vec_get(args, i)));
  }
  return &none_obj;
}