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
  struct num_obj_ctx *ctx = malloc(sizeof(struct num_obj_ctx));
  ctx->is_float = is_float;
  ctx->val_int = val_int;
  ctx->val_float = val_float;

  struct obj *num = obj_new(OBJ_NUM, ctx, &number_type_obj);
  num->ops = &number_builtin_ops;
  num->lazy_load_fn = obj_num_lazy_load;
  return num;
}

static void obj_num_lazy_load(struct obj *num) {
  obj_set_attrib(num, "toString", obj_builtin_new(toString, num));
}

int obj_num_val(struct obj *obj) {
  struct num_obj_ctx *ctx = obj->ctx;
  if (ctx->is_float) {
    return (int)ctx->val_float;
  }
  return ctx->val_int;
}

float obj_num_val_float(struct obj *obj) {
  struct num_obj_ctx *ctx = obj->ctx;
  if (!ctx->is_float) {
    return (float)ctx->val_int;
  }
  return ctx->val_float;
}

static struct obj *__add__(struct obj *num, struct vm *vm, struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return obj_num_new(true, 0, left + right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return obj_num_new(false, left + right, 0);
  }
}

static struct obj *__div__(struct obj *num, struct vm *vm, struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return obj_num_new(true, 0, left / right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return obj_num_new(false, left / right, 0);
  }
}

static struct obj *__eq__(struct obj *num, struct vm *vm, struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return bool_to_obj(left == right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return bool_to_obj(left == right);
  }
}

static struct obj *__greater__(struct obj *num, struct vm *vm,
                               struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return bool_to_obj(left > right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return bool_to_obj(left > right);
  }
}

static struct obj *__lesser__(struct obj *num, struct vm *vm,
                              struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return bool_to_obj(left < right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return bool_to_obj(left < right);
  }
}

static struct obj *__mod__(struct obj *num, struct vm *vm, struct vec *args) {
  int left = obj_num_val(num);
  int right = obj_num_val(vec_get(args, 0));
  return obj_num_new(false, left % right, 0);
}

static struct obj *__mul__(struct obj *num, struct vm *vm, struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return obj_num_new(true, 0, left * right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return obj_num_new(false, left * right, 0);
  }
}

static struct obj *__sub__(struct obj *num, struct vm *vm, struct vec *args) {
  if (obj_num_is_float(num)) {
    float left = obj_num_val_float(num);
    float right = obj_num_val_float(vec_get(args, 0));
    return obj_num_new(true, 0, left - right);
  } else {
    int left = obj_num_val(num);
    int right = obj_num_val(vec_get(args, 0));
    return obj_num_new(false, left - right, 0);
  }
}

static struct obj *toString(struct obj *num, struct vm *vm, struct vec *args) {
  char str[64];
  if (obj_num_is_float(num)) {
    sprintf(str, "%f", obj_num_val_float(num));
  } else {
    sprintf(str, "%d", obj_num_val(num));
  }
  return obj_string_new(str);
}