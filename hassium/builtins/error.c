#include <builtins.h>

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