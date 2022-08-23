#include <object.h>

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *__index__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__slice__(struct obj *, struct vm *, struct vec *);
static struct obj *__storeindex__(struct obj *, struct vm *, struct vec *);
static struct obj *each(struct obj *, struct vm *, struct vec *);
static struct obj *filter(struct obj *, struct vm *, struct vec *);
static struct obj *filterMap(struct obj *, struct vm *, struct vec *);
static struct obj *join(struct obj *, struct vm *, struct vec *);
static struct obj *length(struct obj *, struct vm *, struct vec *);
static struct obj *map(struct obj *, struct vm *, struct vec *);
static struct obj *peek(struct obj *, struct vm *, struct vec *);
static struct obj *pop(struct obj *, struct vm *, struct vec *);
static struct obj *push(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

static struct builtin_ops array_builtin_ops = {
    .__add__ = __add__,
    .__iter__ = __iter__,
};

struct obj *obj_array_new(struct vec *items) {
  items->grow_with = &none_obj;

  for (int i = 0; i < items->len; i++) {
    obj_inc_ref(vec_get(items, i));
  }

  struct obj *arr = obj_new(OBJ_ARRAY, items, &array_type_obj);

  arr->ops = &array_builtin_ops;

  obj_set_attrib(arr, "__index__", obj_builtin_new(__index__, arr));
  obj_set_attrib(arr, "__slice__", obj_builtin_new(__slice__, arr));
  obj_set_attrib(arr, "__storeindex__", obj_builtin_new(__storeindex__, arr));
  obj_set_attrib(arr, "each", obj_builtin_new(each, arr));
  obj_set_attrib(arr, "filter", obj_builtin_new(filter, arr));
  obj_set_attrib(arr, "filterMap", obj_builtin_new(filterMap, arr));
  obj_set_attrib(arr, "join", obj_builtin_new(join, arr));
  obj_set_attrib(arr, "len", obj_builtin_new(length, arr));
  obj_set_attrib(arr, "map", obj_builtin_new(map, arr));
  obj_set_attrib(arr, "peek", obj_builtin_new(peek, arr));
  obj_set_attrib(arr, "pop", obj_builtin_new(pop, arr));
  obj_set_attrib(arr, "push", obj_builtin_new(push, arr));
  obj_set_attrib(arr, "toString", obj_builtin_new(toString, arr));

  return arr;
}

int obj_array_len(struct obj *array) { return ((struct vec *)array->ctx)->len; }

static struct obj *__add__(struct obj *array, struct vm *vm, struct vec *args) {
  struct vec *left = array->ctx;
  struct obj *arg1 = vec_get(args, 0);
  if (arg1->type != OBJ_ARRAY) {
    printf("Cannot use %d with array.__add__\n", arg1->type);
    exit(-1);
  }
  struct vec *right = arg1->ctx;
  // int new_len = left->len + right->len;

  // struct vec *new_items = malloc(sizeof(struct vec));
  // new_items->len = new_len;
  // new_items->size = new_len + VEC_EXPAND_AT;
  // new_items->data = malloc(new_len * sizeof(struct obj *));

  // memcpy(new_items->data, left->data, left->len);
  // memcpy(new_items->data + left->len, right->data, right->len);

  struct vec *new_items = vec_new();
  for (int i = 0; i < left->len; i++) {
    vec_push(new_items, vec_get(left, i));
  }
  for (int i = 0; i < right->len; i++) {
    vec_push(new_items, vec_get(right, i));
  }

  return obj_array_new(new_items);
}

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

static struct obj *__slice__(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct obj *arg1 = vec_get(args, 0);
  struct obj *arg2 = vec_get(args, 1);

  int lower = 0;
  int upper = vec->len;

  if (arg1 != NULL && arg1 != &none_obj) {
    lower = obj_num_val(arg1);
  }
  if (arg2 != NULL && arg2 != &none_obj) {
    upper = obj_num_val(arg2);
    if (upper < 0) {
      upper = vec->len - (-1 * upper);
    }
  }

  struct vec *new = vec_new();
  for (int i = lower; i < upper; i++) {
    vec_push(new, vec_get(vec, i));
  }
  return obj_array_new(new);
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
  for (int i = 0; i < vec->len; ++i) {
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

static struct obj *join(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct strbuf *strbuf = strbuf_new();

  char *sep = NULL;
  if (args->len == 1) {
    struct obj *arg1 = vec_get(args, 0);
    sep = arg1->ctx;
  }

  for (int i = 0; i < vec->len; ++i) {
    strbuf_append_str(strbuf, (char *)obj_to_string(vec_get(vec, i), vm)->ctx);
    if (sep != NULL && i != vec->len - 1) {
      strbuf_append_str(strbuf, sep);
    }
  }

  char *new_str = strbuf_done(strbuf);
  struct obj *string = obj_string_new(new_str);
  free(new_str);
  return string;
}

static struct obj *length(struct obj *arr, struct vm *vm, struct vec *args) {
  return obj_num_new(false, obj_array_len(arr), 0);
}

static struct obj *map(struct obj *arr, struct vm *vm, struct vec *args) {
  struct vec *vec = arr->ctx;
  struct vec *new = vec_new();
  struct obj *func = vec_get(args, 0);

  struct vec *map_args = vec_new();
  for (int i = 0; i < vec->len; ++i) {
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

static struct obj *toString(struct obj *arr, struct vm *vm, struct vec *args) {
  struct strbuf *strbuf = strbuf_new();
  strbuf_append(strbuf, '[');

  struct vec *vec = arr->ctx;
  for (int i = 0; i < vec->len; ++i) {
    struct obj *item = vec_get(vec, i);
    if (item->lazy_load_fn) {
      item->lazy_load_fn(item);
      item->lazy_load_fn = NULL;
    }

    struct obj *toString_res = obj_inc_ref(obj_to_string(item, vm));

    if (item->type == OBJ_STRING) {
      strbuf_append(strbuf, '"');
    }
    strbuf_append_str(strbuf, (char *)toString_res->ctx);
    if (item->type == OBJ_STRING) {
      strbuf_append(strbuf, '"');
    }
    if (i + 1 != vec->len) {
      strbuf_append(strbuf, ',');
      strbuf_append(strbuf, ' ');
    }
    obj_dec_ref(toString_res);
  }

  strbuf_append(strbuf, ']');
  char *str = strbuf_done(strbuf);
  struct obj *ret = obj_string_new(str);
  free(str);
  return ret;
}