#include <builtins.h>

static struct obj *__add__(struct obj *, struct vm *, struct vec *);
static struct obj *__div__(struct obj *, struct vm *, struct vec *);
static struct obj *__eq__(struct obj *, struct vm *, struct vec *);
static struct obj *__greater__(struct obj *, struct vm *, struct vec *);
static struct obj *__mul__(struct obj *, struct vm *, struct vec *);
static struct obj *__sub__(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_num_new(float value) {
  struct num_obj_ctx *ctx =
      (struct num_obj_ctx *)malloc(sizeof(struct num_obj_ctx));
  ctx->value = value;
  struct obj *num = obj_new(OBJ_NUM, ctx);
  obj_setattr(num, "__add__", obj_builtin_new(__add__, num));
  obj_setattr(num, "__div__", obj_builtin_new(__div__, num));
  obj_setattr(num, "__eq__", obj_builtin_new(__eq__, num));
  obj_setattr(num, "__greater__", obj_builtin_new(__greater__, num));
  obj_setattr(num, "__mul__", obj_builtin_new(__mul__, num));
  obj_setattr(num, "__sub__", obj_builtin_new(__sub__, num));
  obj_setattr(num, "toString", obj_builtin_new(toString, num));
  return num;
}

float obj_num_val(struct obj *obj) {
  struct num_obj_ctx *ctx = obj->ctx;
  return ctx->value;
}

static struct obj *__add__(struct obj *num, struct vm *vm, struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left + right);
}

static struct obj *__div__(struct obj *num, struct vm *vm, struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left / right);
}

static struct obj *__eq__(struct obj *num, struct vm *vm, struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left == right);
}

static struct obj *__greater__(struct obj *num, struct vm *vm,
                               struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left > right);
}

static struct obj *__mul__(struct obj *num, struct vm *vm, struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left * right);
}

static struct obj *__sub__(struct obj *num, struct vm *vm, struct vec *args) {
  float left = obj_num_val(num);
  float right = obj_num_val(vec_get(args, 0));
  return obj_num_new(left - right);
}

static struct obj *toString(struct obj *num, struct vm *vm, struct vec *args) {
  char str[64];
  sprintf(str, "%f", obj_num_val(num));
  return obj_string_new(str);
}