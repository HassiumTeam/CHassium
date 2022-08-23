#include <builtins.h>

static void obj_num_lazy_load(struct obj *);

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *__div__(struct obj *, struct vm *, struct vec *);
static struct obj *__eq__(struct obj *, struct vm *, struct vec *);
static struct obj *__greater__(struct obj *, struct vm *, struct vec *);
static struct obj *__lesser__(struct obj *, struct vm *, struct vec *);
static struct obj *__mod__(struct obj *, struct vm *, struct vec *);
static struct obj *__mul__(struct obj *, struct vm *, struct vec *);
static struct obj *__sub__(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

static struct builtin_ops number_builtin_ops = {
    .__add__ = __add__,
    .__div__ = __div__,
    .__eq__ = __eq__,
    .__greater__ = __greater__,
    .__lesser__ = __lesser__,
    .__mod__ = __mod__,
    .__mul__ = __mul__,
    .__sub__ = __sub__,
};

struct obj *obj_num_new(bool is_float, int val_int, float val_float) {
  struct obj *num =
      obj_new(OBJ_NUM, (void *)(uintptr_t)val_int, &number_type_obj);

  num->ops = &number_builtin_ops;
  num->lazy_load_fn = obj_num_lazy_load;

  return num;
}

static void obj_num_lazy_load(struct obj *num) {
  obj_set_attrib(num, "toString", obj_builtin_new(toString, num));
}

static struct obj *__add__(struct obj *num, struct vm *vm, struct vec *args) {
  struct obj *arg1 = vec_get(args, 0);

  if (arg1->type == OBJ_STRING) {
    struct vec args;
    args.len = 0;
    struct obj *toString_res = toString(num, vm, &args);

    char *left = toString_res->ctx;
    int left_len = strlen(left);
    char *right = arg1->ctx;
    int right_len = strlen(right);

    char new_str[left_len + right_len + 1];
    memcpy(new_str, left, left_len);
    memcpy(new_str + left_len, right, right_len);
    new_str[left_len + right_len] = 0;

    obj_dec_ref(toString_res);

    return obj_string_new(new_str);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(arg1);
    return obj_num_new(false, left + right, 0);
  }
}

static struct obj *__div__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return obj_num_new(false, left / right, 0);
}

static struct obj *__eq__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return bool_to_obj(left == right);
}

static struct obj *__greater__(struct obj *num, struct vm *vm,
                               struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return bool_to_obj(left > right);
}

static struct obj *__lesser__(struct obj *num, struct vm *vm,
                              struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return bool_to_obj(left < right);
}

static struct obj *__mod__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return obj_num_new(false, left % right, 0);
}

static struct obj *__mul__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return obj_num_new(false, left * right, 0);
}

static struct obj *__sub__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val((struct obj *)vec_get(args, 0));
  return obj_num_new(false, left - right, 0);
}

static struct obj *toString(struct obj *num, struct vm *vm, struct vec *args) {
  char str[64];
  sprintf(str, "%ld", obj_num_val(num));
  return obj_string_new(str);
}