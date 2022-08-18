#include <builtins.h>

static struct obj *__eq__(struct obj *, struct vm *, struct vec *);

void obj_bool_init(struct obj *obj) {
  obj->attribs = obj_hashmap_new();
  obj_set_attrib(obj, "__eq__", obj_builtin_new(__eq__, obj));
}

void obj_bool_free(struct obj *obj) {
  if (obj->weak_refs != NULL) {
    struct obj **ref;
    for (int i = 0; i < obj->weak_refs->len; i++) {
      ref = vec_get(obj->weak_refs, i);
      *ref = &none_obj;
    }
    vec_free(obj->weak_refs);
  }
  obj_hashmap_free(obj->attribs);
}

bool obj_is_true(struct obj *obj, struct vm *vm) {
  if (obj == &true_obj) return true;
  if (obj == &false_obj) return false;
  if (obj->type == OBJ_NUM) {
    int val = obj_num_val(obj);
    return val != 0;
  }
  return obj->type != OBJ_NONE;
}

static struct obj *__eq__(struct obj *bool_, struct vm *vm, struct vec *args) {
  return bool_to_obj(obj_is_true(bool_, vm) ==
                     obj_is_true(vec_get(args, 0), vm));
}