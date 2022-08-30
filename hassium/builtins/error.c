#include <builtins.h>

struct obj *obj_arg_mismatch_error_new(struct vm *vm, struct obj *target,
                                       struct obj *expecting, struct obj *got) {
  struct obj *error = obj_new(OBJ_ANON, NULL, &arg_mismatch_error_type_obj);
  obj_set_attrib(error, "target", target);
  obj_set_attrib(error, "expecting", expecting);
  obj_set_attrib(error, "got", got);
  obj_set_attrib(error, "toString", obj_builtin_new(Error_toString, error));

  struct strbuf *strbuf = strbuf_new();
  struct obj *target_toString = obj_to_string(target, vm);
  struct obj *expecting_toString = obj_to_string(expecting, vm);
  struct obj *got_toString = obj_to_string(got, vm);

  strbuf_append_str(strbuf, target_toString->ctx);
  strbuf_append_str(strbuf, " expected ");
  strbuf_append_str(strbuf, expecting_toString->ctx);
  strbuf_append_str(strbuf, " got ");
  strbuf_append_str(strbuf, got_toString->ctx);

  obj_dec_ref(target_toString);
  obj_dec_ref(expecting_toString);
  obj_dec_ref(got_toString);

  char *msg_str = strbuf_done(strbuf);
  obj_set_attrib(error, "message", obj_string_new(msg_str));
  free(msg_str);

  return error;
}

struct obj *obj_name_error_new(struct obj *name) {
  struct obj *error = obj_new(OBJ_ANON, NULL, &name_error_type_obj);
  obj_set_attrib(error, "name", name);
  obj_set_attrib(error, "toString", obj_builtin_new(Error_toString, error));
  struct strbuf *strbuf = strbuf_new();
  strbuf_append_str(strbuf, (char *)name->ctx);
  strbuf_append_str(strbuf, " is not defined");

  char *msg_str = strbuf_done(strbuf);
  obj_set_attrib(error, "message", obj_string_new(msg_str));
  free(msg_str);

  return error;
}

struct obj *obj_type_error_new(struct obj *target, struct obj *expected,
                               struct obj *got) {
  struct obj *type_error = obj_new(OBJ_ANON, NULL, &type_error_type_obj);
  obj_set_attrib(type_error, "target", target);
  obj_set_attrib(type_error, "expected", expected);
  obj_set_attrib(type_error, "got", got);
  obj_set_attrib(type_error, "toString",
                 obj_builtin_new(Error_toString, type_error));

  struct strbuf *strbuf = strbuf_new();
  strbuf_append_str(strbuf, "Expected type: ");
  strbuf_append_str(strbuf, (char *)expected->ctx);
  strbuf_append_str(strbuf, ", got type: ");
  strbuf_append_str(strbuf, (char *)got->ctx);

  char *msg_str = strbuf_done(strbuf);
  obj_set_attrib(type_error, "message", obj_string_new(msg_str));
  free(msg_str);

  return type_error;
}

struct obj *Error_toString(struct obj *error, struct vm *vm, struct vec *args) {
  struct obj *message = obj_hashmap_get(error->attribs, "message");
  bool has_message =
      message != &none_obj && message->obj_type == &string_type_obj;

  struct strbuf *strbuf = strbuf_new();
  strbuf_append_str(strbuf, "Type: ");
  strbuf_append_str(strbuf, (char *)error->obj_type->ctx);
  strbuf_append(strbuf, '\n');
  if (has_message) {
    strbuf_append_str(strbuf, "Message:\n");
    strbuf_append_str(strbuf, (char *)message->ctx);
  }

  char *error_str = strbuf_done(strbuf);
  struct obj *ret = obj_string_new(error_str);
  free(error_str);

  return ret;
}