#include <builtins.h>

static struct obj *println(struct obj *, struct vm *, struct vec *);
static struct obj *typeof_(struct obj *, struct vm *, struct vec *);
static struct obj *values(struct obj *, struct vm *, struct vec *);

struct hashmap *get_defaults() {
  struct hashmap *defaults = obj_hashmap_new();
  obj_hashmap_set(defaults, "array", &array_type_obj);
  obj_hashmap_set(defaults, "bool", &bool_type_obj);
  obj_hashmap_set(defaults, "builtin", &builtin_type_obj);
  obj_hashmap_set(defaults, "func", &func_type_obj);
  obj_hashmap_set(defaults, "iter", &iter_type_obj);
  obj_hashmap_set(defaults, "keys",
                  obj_inc_ref(obj_builtin_new(object_keys, NULL)));
  obj_hashmap_set(defaults, "none", &none_type_obj);
  obj_hashmap_set(defaults, "number", &number_type_obj);
  obj_hashmap_set(defaults, "object", &object_type_obj);
  obj_hashmap_set(defaults, "println",
                  obj_inc_ref(obj_builtin_new(println, NULL)));
  obj_hashmap_set(defaults, "string", &string_type_obj);
  obj_hashmap_set(defaults, "type", &type_type_obj);
  obj_hashmap_set(defaults, "typeof",
                  obj_inc_ref(obj_builtin_new(typeof_, NULL)));
  obj_hashmap_set(defaults, "values",
                  obj_inc_ref(obj_builtin_new(values, NULL)));
  obj_hashmap_set(defaults, "weakref", &weakref_type_obj);
  return defaults;
}

static void get_keys(void *key, size_t ksize, uintptr_t value, void *usr) {
  struct vec *vec = usr;
  vec_push(vec, obj_string_new(key));
}

struct obj *object_keys(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *target = vec_get(args, 0);
  struct vec *keys = vec_new();
  if (target->attribs != NULL) {
    hashmap_iterate(target->attribs, get_keys, keys);
  }
  return obj_array_new(keys);
}

static struct obj *println(struct obj *_, struct vm *vm, struct vec *args) {
  for (int i = 0; i < args->len; i++) {
    struct obj *arg = vec_get(args, i);
    if (arg->type == OBJ_STRING)
      printf("%s", (char *)arg->ctx);
    else if (arg == &none_obj)
      printf("none");
    else if (arg == &true_obj)
      printf("true");
    else if (arg == &false_obj)
      printf("false");
    else if (arg->obj_type == &type_type_obj)
      printf("%s", (char *)arg->ctx);
    else {
      struct obj *str = obj_to_string(arg, vm);
      printf("%s", (char *)str->ctx);
      obj_dec_ref(str);
    }
  }
  printf("\n");
  return &none_obj;
}

static struct obj *typeof_(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *obj = vec_get(args, 0);
  return obj->obj_type;
}

static struct obj *values(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *target = vec_get(args, 0);
  struct vec *keys = vec_new();
  struct vec *values = vec_new();

  hashmap_iterate(target->attribs, get_keys, keys);
  for (int i = 0; i < keys->len; i++) {
    vec_push(values, obj_hashmap_get(target->attribs, vec_get(keys, i)));
  }

  for (int i = 0; i < keys->len; i++) {
    obj_dec_ref(vec_get(keys, i));
  }
  vec_free(keys);

  return obj_array_new(values);
}