#include <object.h>

static struct obj *__index__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__storeindex__(struct obj *, struct vm *, struct vec *);
static struct obj *each(struct obj *, struct vm *, struct vec *);
static struct obj *filter(struct obj *, struct vm *, struct vec *);
static struct obj *filterMap(struct obj *, struct vm *, struct vec *);
static struct obj *length(struct obj *, struct vm *, struct vec *);
static struct obj *map(struct obj *, struct vm *, struct vec *);
static struct obj *peek(struct obj *, struct vm *, struct vec *);
static struct obj *pop(struct obj *, struct vm *, struct vec *);
static struct obj *push(struct obj *, struct vm *, struct vec *);

struct obj *obj_array_new(struct vec *items) {
  items->grow_with = &none_obj;
  struct obj *arr = obj_new(OBJ_ARRAY, items, &array_type_obj);
  for (int i = 0; i < items->len; i++) obj_inc_ref(vec_get(items, i));

  obj_set_attrib(arr, "__index__", obj_builtin_new(__index__, arr));
  obj_set_attrib(arr, "__iter__", obj_builtin_new(__iter__, arr));
  obj_set_attrib(arr, "__storeindex__", obj_builtin_new(__storeindex__, arr));
  obj_set_attrib(arr, "each", obj_builtin_new(each, arr));
  obj_set_attrib(arr, "filter", obj_builtin_new(filter, arr));
  obj_set_attrib(arr, "filterMap", obj_builtin_new(filterMap, arr));
  obj_set_attrib(arr, "len", obj_builtin_new(length, arr));
  obj_set_attrib(arr, "map", obj_builtin_new(map, arr));
  obj_set_attrib(arr, "peek", obj_builtin_new(peek, arr));
  obj_set_attrib(arr, "pop", obj_builtin_new(pop, arr));
  obj_set_attrib(arr, "push", obj_builtin_new(push, arr));
  return arr;
}

int obj_array_len(struct obj *array) { return ((struct vec *)array->ctx)->len; }

static struct obj *__index__(struct obj *arr, struct vm *vm, struct vec *args) {
  struct obj *key = vec_get(args, 0);
  if (key->type != OBJ_NUM) {
    printf("Must index array by number!");
    exit(-1);
  }
  int idx = obj_num_val(key);
  return vec_get((struct vec *)arr->ctx, idx);
}

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

static struct obj *__storeindex__(struct obj *arr, struct vm *vm,
                                  struct vec *args) {
  struct vec *vec = arr->ctx;
  int key = obj_num_val((struct obj *)vec_get(args, 0));
  struct obj *val = vec_get(args, 1);

  if (key < vec->len) {
    struct obj *cur = vec_get(vec, key);
    obj_down_ref(cur);
    vec_set(vec, key, obj_inc_ref(val));

  } else {
    vec_set(vec, key, obj_inc_ref(val));
  }
}

static struct obj *each(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct obj *func = vec_get(args, 0);

  struct vec *each_args = vec_new();
  for (int i = 0; i < vec->len; i++) {
    vec_push(each_args, vec_get(vec, i));
    obj_invoke(func, vm, each_args);
    vec_pop(each_args);
  }
  vec_free(each_args);

  return arr;
}

static struct obj *filter(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct vec *new = vec_new();
  struct obj *func = vec_get(args, 0);

  struct vec *filter_args = vec_new();
  for (int i = 0; i < vec->len; i++) {
    struct obj *item = vec_get(vec, i);
    vec_push(filter_args, item);
    if (obj_is_true(obj_invoke(func, vm, filter_args), vm)) {
      vec_push(new, item);
    }
    vec_pop(filter_args);
  }
  vec_free(filter_args);

  return obj_array_new(new);
}

static struct obj *filterMap(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct vec *new = vec_new();
  struct obj *filter_func = vec_get(args, 0);
  struct obj *map_func = vec_get(args, 1);

  struct vec *func_args = vec_new();
  for (int i = 0; i < vec->len; i++) {
    struct obj *item = vec_get(vec, i);
    vec_push(func_args, item);
    if (obj_is_true(obj_invoke(filter_func, vm, func_args), vm)) {
      vec_push(new, obj_invoke(map_func, vm, func_args));
    }
    vec_pop(func_args);
  }
  vec_free(func_args);

  return obj_array_new(new);
}

static struct obj *length(struct obj *arr, struct vm *vm, struct vec *args) {
  return obj_num_new(false, obj_array_len(arr), 0);
}

static struct obj *map(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct vec *new = vec_new();
  struct obj *func = vec_get(args, 0);

  struct vec *map_args = vec_new();
  for (int i = 0; i < vec->len; i++) {
    vec_push(map_args, vec_get(vec, i));
    vec_push(new, obj_invoke(func, vm, map_args));
    vec_pop(map_args);
  }
  vec_free(map_args);

  return obj_array_new(new);
}

static struct obj *peek(struct obj *arr, struct vm *vm, struct vec *args) {
  if (obj_array_len(arr) == 0) return &none_obj;
  return vec_peek((struct vec *)arr->ctx);
}

static struct obj *pop(struct obj *arr, struct vm *vm, struct vec *args) {
  if (obj_array_len(arr) == 0) return &none_obj;
  return obj_down_ref(vec_pop((struct vec *)arr->ctx));
}

static struct obj *push(struct obj *arr, struct vm *vm, struct vec *args) {
  for (int i = 0; i < args->len; i++) {
    vec_push(arr->ctx, obj_inc_ref(vec_get(args, i)));
  }
  return &none_obj;
}