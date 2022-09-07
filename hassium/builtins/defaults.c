#include <builtins.h>

static struct obj *type_toString(struct obj *, struct vm *, struct vec *);
static struct obj *ArgMismatchError(struct obj *, struct vm *, struct vec *);
static struct obj *Array(struct obj *, struct vm *, struct vec *);
static struct obj *Bool(struct obj *, struct vm *, struct vec *);
static struct obj *FileNotFoundError(struct obj *, struct vm *, struct vec *);
static struct obj *NameError(struct obj *, struct vm *, struct vec *);
static struct obj *NoSuchAttribError(struct obj *, struct vm *, struct vec *);
static struct obj *Number(struct obj *, struct vm *, struct vec *);
static struct obj *String(struct obj *, struct vm *, struct vec *);
static struct obj *TypeError(struct obj *, struct vm *, struct vec *);

static struct obj *println(struct obj *, struct vm *, struct vec *);
static struct obj *typeof_(struct obj *, struct vm *, struct vec *);
static struct obj *values(struct obj *, struct vm *, struct vec *);

struct hashmap *get_defaults() {
  obj_set_attrib(&arg_mismatch_error_type_obj, "new",
                 obj_builtin_new(ArgMismatchError, NULL));
  obj_set_attrib(&arg_mismatch_error_type_obj, "toString",
                 obj_builtin_new(type_toString, &arg_mismatch_error_type_obj));

  obj_set_attrib(&array_type_obj, "new", obj_builtin_new(Array, NULL));
  obj_set_attrib(&array_type_obj, "toString",
                 obj_builtin_new(type_toString, &array_type_obj));

  obj_set_attrib(&bool_type_obj, "new", obj_builtin_new(Bool, NULL));
  obj_set_attrib(&bool_type_obj, "toString",
                 obj_builtin_new(type_toString, &bool_type_obj));

  obj_set_attrib(&builtin_type_obj, "toString",
                 obj_builtin_new(type_toString, &builtin_type_obj));

  obj_set_attrib(&compile_error_type_obj, "toString",
                 obj_builtin_new(type_toString, &compile_error_type_obj));

  obj_set_attrib(&file_not_found_error_type_obj, "new",
                 obj_builtin_new(FileNotFoundError, NULL));
  obj_set_attrib(
      &file_not_found_error_type_obj, "toString",
      obj_builtin_new(type_toString, &file_not_found_error_type_obj));

  obj_set_attrib(&func_type_obj, "toString",
                 obj_builtin_new(type_toString, &func_type_obj));

  obj_set_attrib(&iter_type_obj, "toString",
                 obj_builtin_new(type_toString, &iter_type_obj));

  obj_set_attrib(&name_error_type_obj, "new", obj_builtin_new(NameError, NULL));
  obj_set_attrib(&name_error_type_obj, "toString",
                 obj_builtin_new(type_toString, &name_error_type_obj));

  obj_set_attrib(&no_such_attrib_error_type_obj, "new",
                 obj_builtin_new(NoSuchAttribError, NULL));
  obj_set_attrib(
      &no_such_attrib_error_type_obj, "toString",
      obj_builtin_new(type_toString, &no_such_attrib_error_type_obj));

  obj_set_attrib(&number_type_obj, "toString",
                 obj_builtin_new(type_toString, &number_type_obj));

  obj_set_attrib(&string_type_obj, "new", obj_builtin_new(String, NULL));
  obj_set_attrib(&string_type_obj, "toString",
                 obj_builtin_new(type_toString, &string_type_obj));

  obj_set_attrib(&type_error_type_obj, "new", obj_builtin_new(TypeError, NULL));
  obj_set_attrib(&type_error_type_obj, "toString",
                 obj_builtin_new(type_toString, &type_error_type_obj));

  obj_bool_init(&true_obj);
  obj_bool_init(&false_obj);

  struct hashmap *defaults = obj_hashmap_new();
  obj_hashmap_set(defaults, "ArgMismatchError", &arg_mismatch_error_type_obj);
  obj_hashmap_set(defaults, "Array", &array_type_obj);
  obj_hashmap_set(defaults, "Bool", &bool_type_obj);
  obj_hashmap_set(defaults, "Builtin", &builtin_type_obj);
  obj_hashmap_set(defaults, "CompileError", &compile_error_type_obj);
  obj_hashmap_set(defaults, "FileNotFoundError",
                  &file_not_found_error_type_obj);
  obj_hashmap_set(defaults, "Func", &func_type_obj);
  obj_hashmap_set(defaults, "Iter", &iter_type_obj);
  obj_hashmap_set(
      defaults, "keys",
      obj_inc_ref(obj_builtin_new_named(object_keys, NULL, "keys")));
  obj_hashmap_set(defaults, "NameError", &name_error_type_obj);
  obj_hashmap_set(defaults, "NoSuchAttribError",
                  &no_such_attrib_error_type_obj);
  obj_hashmap_set(defaults, "none", &none_type_obj);
  obj_hashmap_set(defaults, "Number", &number_type_obj);
  obj_hashmap_set(defaults, "Object", &object_type_obj);
  obj_hashmap_set(defaults, "println",
                  obj_inc_ref(obj_builtin_new_named(println, NULL, "println")));
  obj_hashmap_set(defaults, "String", &string_type_obj);
  obj_hashmap_set(defaults, "Type", &type_type_obj);
  obj_hashmap_set(defaults, "TypeError", &type_error_type_obj);
  obj_hashmap_set(defaults, "typeof",
                  obj_inc_ref(obj_builtin_new_named(typeof_, NULL, "typeof")));
  obj_hashmap_set(defaults, "values",
                  obj_inc_ref(obj_builtin_new_named(values, NULL, "values")));
  obj_hashmap_set(defaults, "Weakref", &weakref_type_obj);

  return defaults;
}

void destruct_defaults() {
  obj_hashmap_free(arg_mismatch_error_type_obj.attribs);
  vec_free(arg_mismatch_error_type_obj.weak_refs);
  obj_hashmap_free(array_type_obj.attribs);
  vec_free(array_type_obj.weak_refs);
  obj_hashmap_free(bool_type_obj.attribs);
  vec_free(bool_type_obj.weak_refs);
  obj_hashmap_free(builtin_type_obj.attribs);
  vec_free(builtin_type_obj.weak_refs);
  obj_hashmap_free(compile_error_type_obj.attribs);
  vec_free(compile_error_type_obj.weak_refs);
  obj_hashmap_free(file_not_found_error_type_obj.attribs);
  vec_free(file_not_found_error_type_obj.weak_refs);
  obj_hashmap_free(func_type_obj.attribs);
  vec_free(func_type_obj.weak_refs);
  obj_hashmap_free(iter_type_obj.attribs);
  vec_free(iter_type_obj.weak_refs);
  obj_hashmap_free(name_error_type_obj.attribs);
  vec_free(name_error_type_obj.weak_refs);
  obj_hashmap_free(no_such_attrib_error_type_obj.attribs);
  vec_free(no_such_attrib_error_type_obj.weak_refs);
  obj_hashmap_free(number_type_obj.attribs);
  vec_free(number_type_obj.weak_refs);
  obj_hashmap_free(string_type_obj.attribs);
  vec_free(string_type_obj.weak_refs);
  obj_hashmap_free(type_error_type_obj.attribs);
  vec_free(type_error_type_obj.weak_refs);
  obj_bool_free(&true_obj);
  obj_bool_free(&false_obj);
}

static void get_keys(void *key, size_t ksize, uintptr_t value, void *usr) {
  struct vec *vec = usr;
  vec_push(vec, obj_string_new(key));
}
struct obj *object_keys(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *target = vec_get(args, 0);
  if (target->lazy_load_fn) {
    target->lazy_load_fn(target);
    target->lazy_load_fn = NULL;
  }

  struct vec *keys = vec_new();
  if (target->attribs != NULL) {
    hashmap_iterate(target->attribs, get_keys, keys);
  }
  return obj_array_new(keys);
}

static struct obj *ArgMismatchError(struct obj *_, struct vm *vm,
                                    struct vec *args) {
  struct obj *target = vec_get(args, 0);
  struct obj *expecting = vec_get(args, 1);
  struct obj *got = vec_get(args, 2);

  return obj_arg_mismatch_error_new(vm, target, expecting, got);
}

static struct obj *Array(struct obj *_, struct vm *vm, struct vec *args) {
  struct vec *vec = vec_new();
  for (int i = 0; i < args->len; ++i) {
    vec_push(vec, vec_get(args, i));
  }
  return obj_array_new(vec);
}

static struct obj *Bool(struct obj *_, struct vm *vm, struct vec *args) {
  return bool_to_obj(obj_is_true(vec_get(args, 0), vm));
}

static struct obj *FileNotFoundError(struct obj *_, struct vm *vm,
                                     struct vec *args) {
  struct obj *path = obj_enforce_type(vec_get(args, 0), &string_type_obj, vm);
  return obj_file_not_found_error_new(path);
}

static struct obj *NameError(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *name = obj_enforce_type(vec_get(args, 0), &string_type_obj, vm);
  return obj_name_error_new(name);
}

static struct obj *NoSuchAttribError(struct obj *_, struct vm *vm,
                                     struct vec *args) {
  struct obj *target = vec_get(args, 0);
  struct obj *attrib = obj_enforce_type(vec_get(args, 1), &string_type_obj, vm);
  return obj_no_such_attrib_error_new(target, attrib);
}

static struct obj *println(struct obj *_, struct vm *vm, struct vec *args) {
  for (int i = 0; i < args->len; ++i) {
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

static struct obj *String(struct obj *_, struct vm *vm, struct vec *args) {
  return obj_to_string(vec_get(args, 0), vm);
}

static struct obj *TypeError(struct obj *_, struct vm *vm, struct vec *args) {
  struct obj *target = vec_get(args, 0);
  struct obj *expecting = vec_get(args, 1);
  struct obj *got = vec_get(args, 2);

  return obj_type_error_new(target, expecting, got);
}

static struct obj *type_toString(struct obj *type, struct vm *vm,
                                 struct vec *args) {
  return obj_string_new(type->ctx);
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
  for (int i = 0; i < keys->len; ++i) {
    vec_push(values, obj_hashmap_get(target->attribs, vec_get(keys, i)));
  }

  for (int i = 0; i < keys->len; ++i) {
    obj_dec_ref(vec_get(keys, i));
  }
  vec_free(keys);

  return obj_array_new(values);
}