#include <builtins.h>

static void obj_string_lazy_load(struct obj *);

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *__eq__(struct obj *, struct vm *, struct vec *);
static struct obj *__greater__(struct obj *, struct vm *, struct vec *);
static struct obj *__lesser__(struct obj *, struct vm *, struct vec *);
static struct obj *__index__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter__(struct obj *, struct vm *, struct vec *);
static struct obj *__iter____iterfull__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__iter____iternext__(struct obj *, struct vm *,
                                        struct vec *);
static struct obj *__slice__(struct obj *, struct vm *, struct vec *);
static struct obj *len(struct obj *, struct vm *, struct vec *);
static struct obj *split(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

static struct builtin_ops string_builtin_ops = {
    .__add__ = __add__,
    .__eq__ = __eq__,
    .__greater__ = __greater__,
    .__lesser__ = __lesser__,
};

struct obj *obj_string_new(char *value) {
  struct obj *string = obj_new(OBJ_STRING, clone_str(value), &string_type_obj);

  string->ops = &string_builtin_ops;
  string->lazy_load_fn = obj_string_lazy_load;

  return string;
}

static void obj_string_lazy_load(struct obj *string) {
  obj_set_attrib(string, "__index__", obj_builtin_new(__index__, string));
  obj_set_attrib(string, "__iter__", obj_builtin_new(__iter__, string));
  obj_set_attrib(string, "__slice__", obj_builtin_new(__slice__, string));
  obj_set_attrib(string, "len",
                 obj_num_new(false, strlen((char *)string->ctx), 0));
  obj_set_attrib(string, "split", obj_builtin_new(split, string));
  obj_set_attrib(string, "toString", obj_builtin_new(toString, string));
}

char *obj_string_val(struct obj *string) { return string->ctx; }

static struct obj *__add__(struct obj *left, struct vm *vm, struct vec *args) {
  struct obj *right = vec_get(args, 0);

  char *left_str = (char *)left->ctx;
  char *right_str = (char *)right->ctx;
  int left_len = strlen(left_str);
  int right_len = strlen(right_str);

  char new_str[left_len + right_len + 1];
  memcpy(new_str, left_str, left_len);
  memcpy(new_str + left_len, right_str, right_len);
  new_str[left_len + right_len] = 0;

  return obj_string_new(new_str);
}

static struct obj *__eq__(struct obj *left, struct vm *vm, struct vec *args) {
  struct obj *right = vec_get(args, 0);
  return bool_to_obj(strcmp((char *)left->ctx, (char *)right->ctx) == 0);
}

static struct obj *__index__(struct obj *string, struct vm *vm,
                             struct vec *args) {
  struct obj *key = vec_get(args, 0);

  if (key->type != OBJ_NUM) {
    printf("Must index string by number!");
    exit(-1);
  }

  int idx = obj_num_val(key);
  char str[2];
  str[0] = ((char *)string->ctx)[idx];
  str[1] = 0;

  return obj_string_new(str);
}

static struct obj *__iter__(struct obj *obj, struct vm *vm, struct vec *args) {
  struct obj *iter = obj_iter_new(obj);

  obj_set_attrib(iter, "__iterfull__",
                 obj_builtin_new(__iter____iterfull__, iter));
  obj_set_attrib(iter, "__iternext__",
                 obj_builtin_new(__iter____iternext__, iter));

  return iter;
}

static struct obj *__iter____iterfull__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  return bool_to_obj(iter->pos >= strlen(iter->target->ctx));
}

static struct obj *__iter____iternext__(struct obj *iter_, struct vm *vm,
                                        struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  char c[2];
  c[0] = ((char *)iter->target->ctx)[iter->pos++];
  c[1] = 0;
  return obj_string_new(c);
}

static struct obj *__greater__(struct obj *left, struct vm *vm,
                               struct vec *args) {
  struct obj *right = vec_get(args, 0);
  return bool_to_obj(strcmp((char *)left->ctx, (char *)right->ctx) > 0);
}

static struct obj *__lesser__(struct obj *left, struct vm *vm,
                              struct vec *args) {
  struct obj *right = vec_get(args, 0);
  return bool_to_obj(strcmp((char *)left->ctx, (char *)right->ctx) < 0);
}

static struct obj *__slice__(struct obj *obj, struct vm *vm, struct vec *args) {
  struct obj *arg1 = vec_get(args, 0);
  struct obj *arg2 = vec_get(args, 1);

  char *string = obj->ctx;
  int len = strlen(string);
  int lower = 0;
  int upper = len;

  if (arg1 != NULL && arg1 != &none_obj) {
    lower = obj_num_val(arg1);
  }
  if (arg2 != NULL && arg2 != &none_obj) {
    upper = obj_num_val(arg2);
    if (upper < 0) {
      upper = len - (-1 * upper);
    }
  }

  char *ret_str = calloc(upper - lower + 1, sizeof(char));
  memcpy(ret_str, string + lower, upper - lower);
  struct obj *ret = obj_string_new(ret_str);
  free(ret_str);
  return ret;
}

static struct obj *split(struct obj *obj, struct vm *vm, struct vec *args) {
  char *str = (char *)obj->ctx;
  int len = strlen(str);
  struct vec *new_items = vec_new();

  if (args->len == 0) {
    for (int i = 0; i < len; i++) {
      char part[2];
      part[0] = str[i];
      part[1] = 0;
      vec_push(new_items, obj_string_new(part));
    }
  } else {
    struct obj *arg1 = vec_get(args, 0);
    char sep = ((char *)arg1->ctx)[0];

    struct strbuf *strbuf = strbuf_new();
    for (int i = 0; i < len; i++) {
      if (str[i] == sep) {
        char *part = strbuf_done(strbuf);
        vec_push(new_items, obj_string_new(part));
        free(part);
        strbuf = strbuf_new();
      } else {
        strbuf_append(strbuf, str[i]);
      }
    }
    vec_push(new_items, obj_string_new(strbuf_done(strbuf)));
  }

  struct obj *array = obj_array_new(new_items);
}

static struct obj *toString(struct obj *string, struct vm *vm,
                            struct vec *args) {
  return string;
}