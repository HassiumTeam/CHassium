#include <builtins.h>

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *__div__(struct obj *, struct vm *, struct vec *);
static struct obj *__eq__(struct obj *, struct vm *, struct vec *);
static struct obj *__greater__(struct obj *, struct vm *, struct vec *);
static struct obj *__lesser__(struct obj *, struct vm *, struct vec *);
static struct obj *__mod__(struct obj *, struct vm *, struct vec *);
static struct obj *__mul__(struct obj *, struct vm *, struct vec *);
static struct obj *__sub__(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_num_new(bool is_float, int val_int, float val_float) {
  struct num_obj_ctx *ctx =
      (struct num_obj_ctx *)malloc(sizeof(struct num_obj_ctx));
  ctx->is_float = is_float;
  ctx->val_int = val_int;
  ctx->val_float = val_float;

  struct obj *num = obj_new(OBJ_NUM, ctx, &number_type_obj);
  obj_set_attrib(num, "__add__", obj_builtin_new(__add__, num));
  obj_set_attrib(num, "__div__", obj_builtin_new(__div__, num));
  obj_set_attrib(num, "__eq__", obj_builtin_new(__eq__, num));
  obj_set_attrib(num, "__greater__", obj_builtin_new(__greater__, num));
  obj_set_attrib(num, "__lesser__", obj_builtin_new(__lesser__, num));
  obj_set_attrib(num, "__mod__", obj_builtin_new(__mod__, num));
  obj_set_attrib(num, "__mul__", obj_builtin_new(__mul__, num));
  obj_set_attrib(num, "__sub__", obj_builtin_new(__sub__, num));
  obj_set_attrib(num, "toString", obj_builtin_new(toString, num));

  return num;
}

bool obj_num_is_float(struct obj *obj) {
  struct num_obj_ctx *num = obj->ctx;
  return num->is_float;
}

int obj_num_val(struct obj *obj) {
  struct num_obj_ctx *ctx = obj->ctx;
  if (ctx->val_float != 0) {
    return (int)ctx->val_float;
  }
  return ctx->val_int;
}

float obj_num_val_float(struct obj *obj) {
  struct num_obj_ctx *ctx = obj->ctx;
  if (ctx->val_int != 0) {
    return (float)ctx->val_float;
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