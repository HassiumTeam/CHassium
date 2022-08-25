#include <builtins.h>

static void obj_func_lazy_load(struct obj *);

static struct obj *bind(struct obj *, struct vm *, struct vec *);
static struct obj *toString(struct obj *, struct vm *, struct vec *);

struct obj *obj_func_new(struct code_obj *code_obj, struct vec *params,
                         struct obj *self, struct stackframe *frame,
                         bool closure) {
  struct func_obj_ctx *ctx = malloc(sizeof(struct func_obj_ctx));
  ctx->code_obj = code_obj;
  ctx->params = params;
  if (self != NULL) {
    ctx->self = obj_inc_ref(obj_weakref_new(self));
  } else {
    ctx->self = NULL;
  }
  if (closure) {
    ctx->frame = stackframe_inc_ref(frame);
  } else {
    ctx->frame = frame;
  }

  struct obj *func = obj_new(OBJ_FUNC, ctx, &func_type_obj);
  func->lazy_load_fn = obj_func_lazy_load;
  return func;
}

static void obj_func_lazy_load(struct obj *func) {
  obj_set_attrib(func, "bind", obj_builtin_new(bind, func));
  obj_set_attrib(func, "toString", obj_builtin_new(toString, func));
}

static struct obj *bind(struct obj *func, struct vm *vm, struct vec *args) {
  struct obj *arg1 = vec_get(args, 0);
  struct func_obj_ctx *ctx = func->ctx;

  struct obj *new = obj_func_new(ctx->code_obj, ctx->params, arg1, ctx->frame,
                                 ctx->frame != NULL);
  return new;
}

static struct obj *toString(struct obj *obj, struct vm *vm, struct vec *args) {
  struct func_obj_ctx *func = obj->ctx;

  return obj_string_new(func->code_obj->name);
}