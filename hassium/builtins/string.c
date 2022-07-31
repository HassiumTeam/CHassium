#include <builtins.h>

struct obj *__iter__(struct obj *, struct vm *, struct vec *);
struct obj *__iter____iterfull__(struct obj *, struct vm *, struct vec *);
struct obj *__iter____iternext__(struct obj *, struct vm *, struct vec *);

struct obj *obj_string_new(char *value) {
  struct obj *string = obj_new(OBJ_STRING, clone_str(value), &string_type_obj);

  obj_set_attrib(string, "__iter__", obj_builtin_new(__iter__, string));

  return string;
}

char *obj_string_val(struct obj *string) { return string->ctx; }

struct obj *__iter__(struct obj *obj, struct vm *vm, struct vec *args) {
  struct obj *iter = obj_iter_new(obj);

  obj_set_attrib(iter, "__iterfull__",
                 obj_builtin_new(__iter____iterfull__, iter));
  obj_set_attrib(iter, "__iternext__",
                 obj_builtin_new(__iter____iternext__, iter));

  return iter;
}

struct obj *__iter____iterfull__(struct obj *iter_, struct vm *vm,
                                 struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  return bool_to_obj(iter->pos >= strlen(iter->target->ctx));
}

struct obj *__iter____iternext__(struct obj *iter_, struct vm *vm,
                                 struct vec *args) {
  struct iter_obj_ctx *iter = iter_->ctx;
  char c[2];
  c[0] = ((char *)iter->target->ctx)[iter->pos++];
  c[1] = 0;
  return obj_string_new(c);
}