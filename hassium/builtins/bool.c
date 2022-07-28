#include <builtins.h>

bool obj_is_true(struct obj *obj, struct vm *vm) {
  if (obj == &true_obj) return true;
  if (obj == &false_obj) return false;
  if (obj->type == OBJ_NUM) {
    float val = obj_num_val(obj);
    return val != 0;
  }
  printf("Not implemented!\n");
  exit(-1);
}

bool obj_is_false(struct obj *obj, struct vm *vm) {
  if (obj == &true_obj) return false;
  if (obj == &false_obj) return true;
  if (obj->type == OBJ_NUM) {
    float val = obj_num_val(obj);
    return val == 0;
  }
  printf("Not implemented!\n");
  exit(-1);
}