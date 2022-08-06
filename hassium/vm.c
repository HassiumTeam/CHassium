#include <vm.h>

static void import_attrib_from_map(void *, size_t, uintptr_t, void *);
static void vm_run_cleanup(struct vec *);

struct vm *vm_new() {
  struct vm *vm = (struct vm *)calloc(1, sizeof(struct vm));
  vm->frames = vec_new();
  vec_push(vm->frames, get_defaults());
  obj_bool_init(&true_obj);
  obj_bool_init(&false_obj);
  return vm;
}

void vm_free(struct vm *vm) {
  for (int i = 0; i < vm->frames->len; i++)
    obj_hashmap_free(vec_get(vm->frames, i));
  vec_free(vm->frames);
  obj_bool_free(&true_obj);
  obj_bool_free(&false_obj);
  free(vm);
}

struct obj *vm_run(struct obj *self, struct vm *vm, struct code_obj *code_obj) {
  struct vec _stack;
  struct vec *stack = vec_init(&_stack);
  struct vm_inst *inst;

  int pos = 0;
  while (pos < code_obj->instructions->len) {
    inst = vec_get(code_obj->instructions, pos);
    // printf("Inst %d\n", inst->type);

    switch (inst->type) {
      case INST_BIN_OP: {
        struct obj *right = vec_pop(stack);
        struct obj *left = vec_pop(stack);
        vec_push(stack, obj_inc_ref(obj_bin_op(
                            ((struct bin_op_inst *)inst->inner)->type, left,
                            right, vm)));
        obj_dec_ref(left);
        obj_dec_ref(right);
      } break;
      case INST_BUILD_ARRAY: {
        struct vec *items = vec_new();
        for (int i = ((struct build_array_inst *)inst->inner)->count - 1;
             i >= 0; i--)
          vec_set(items, i, obj_down_ref(vec_pop(stack)));
        vec_push(stack, obj_inc_ref(obj_array_new(items)));
      } break;
      case INST_BUILD_CLASS: {
        struct build_class_inst *build_class = inst->inner;
        struct obj *class =
            obj_new(OBJ_TYPE, build_class->code_obj->name, &type_type_obj);
        obj_hashmap_set(vec_peek(vm->frames), build_class->code_obj->name,
                        obj_inc_ref(class));
        vec_push(vm->frames, class->attribs);
        vm_run(self, vm, build_class->code_obj);
        vec_pop(vm->frames);

      } break;
      case INST_BUILD_FUNC: {
        struct build_func_inst *build_func = inst->inner;
        vec_push(stack, obj_inc_ref(obj_func_new(build_func->code_obj,
                                                 build_func->params, NULL)));
      } break;
      case INST_BUILD_OBJ: {
        struct obj *new = obj_new(OBJ_ANON, NULL, &object_type_obj);
        struct vec *keys = ((struct build_obj_inst *)inst->inner)->keys;
        for (int i = keys->len - 1; i >= 0; i--)
          obj_set_attrib(new, vec_get(keys, i), obj_down_ref(vec_pop(stack)));
        vec_push(stack, obj_inc_ref(new));
      } break;
      case INST_IMPORT: {
        struct import_inst *import = inst->inner;
        struct hashmap *frame = obj_hashmap_new();
        vec_push(vm->frames, frame);
        vm_run(NULL, vm, import->mod);
        vec_pop(vm->frames);
        if (import->imports->len == 0) {
          hashmap_iterate(frame, import_attrib_from_map, vec_peek(vm->frames));
        } else {
          for (int i = 0; i < import->imports->len; i++) {
            char *attrib = vec_get(import->imports, i);
            import_attrib_from_map(attrib, 0,
                                   (uintptr_t)obj_hashmap_get(frame, attrib),
                                   vec_peek(vm->frames));
          }
        }
        obj_hashmap_free(frame);
      } break;
      case INST_INVOKE: {
        int arg_count = ((struct invoke_inst *)inst->inner)->arg_count;
        struct vec *args = vec_new();
        for (int i = arg_count - 1; i >= 0; i--)
          vec_set(args, i, vec_pop(stack));
        struct obj *target = vec_pop(stack);
        vec_push(stack, obj_inc_ref(obj_invoke(target, vm, args)));
        for (int i = 0; i < arg_count; i++) obj_dec_ref(vec_get(args, i));
        vec_free(args);
        obj_dec_ref(target);
      } break;
      case INST_JUMP:
        pos = intmap_get(code_obj->labels,
                         ((struct jump_inst *)inst->inner)->label);
        break;
      case INST_JUMP_IF_FALSE: {
        struct obj *val = vec_pop(stack);
        if (obj_is_false(val, vm)) {
          pos = intmap_get(code_obj->labels,
                           ((struct jump_inst *)inst->inner)->label);
        }
        obj_dec_ref(val);
      } break;
      case INST_ITER: {
        struct obj *target = vec_pop(stack);
        vec_push(stack,
                 obj_inc_ref(obj_invoke_attrib(target, "__iter__", vm, NULL)));
        obj_dec_ref(target);
      } break;
      case INST_JUMP_IF_FULL: {
        struct obj *iter = vec_pop(stack);
        if (obj_is_true(obj_invoke_attrib(iter, "__iterfull__", vm, NULL),
                        vm)) {
          pos = intmap_get(code_obj->labels,
                           ((struct jump_inst *)inst->inner)->label);
        } else {
          vec_push(stack, obj_inc_ref(obj_invoke_attrib(iter, "__iternext__",
                                                        vm, NULL)));
        }
        obj_dec_ref(iter);
      } break;
      case INST_LOAD_ATTRIB: {
        struct obj *target = vec_pop(stack);
        struct obj *attrib = obj_hashmap_get(
            target->attribs, ((struct load_attrib_inst *)inst->inner)->attrib);
        if (attrib == NULL) attrib = &none_obj;
        vec_push(stack, obj_inc_ref(attrib));
        obj_dec_ref(target);
      } break;
      case INST_LOAD_CONST: {
        struct obj *const_ = vec_get(
            code_obj->consts, ((struct load_const_inst *)inst->inner)->idx);
        vec_push(stack, obj_inc_ref(const_));
      }; break;
      case INST_LOAD_FALSE:
        vec_push(stack, &false_obj);
        break;
      case INST_LOAD_ID: {
        bool found = false;
        char *id = ((struct load_id_inst *)inst->inner)->id;
        for (int i = vm->frames->len - 1; i >= 0; i--) {
          struct hashmap *frame = vec_get(vm->frames, i);
          if (obj_hashmap_has(frame, id)) {
            vec_push(stack, obj_inc_ref(obj_hashmap_get(frame, id)));
            found = true;
            break;
          }
        }
        if (!found) {
          printf("Could not load ID! %s\n",
                 ((struct load_id_inst *)inst->inner)->id);
          exit(-1);
        }
      } break;
      case INST_LOAD_NONE:
        vec_push(stack, &none_obj);
        break;
      case INST_LOAD_SELF:
        vec_push(stack, obj_inc_ref(self));
        break;
      case INST_LOAD_SUBSCRIPT: {
        struct obj *target = vec_pop(stack);
        struct obj *key = vec_pop(stack);
        vec_push(stack, obj_inc_ref(obj_index(target, key, vm)));
        obj_dec_ref(key);
        obj_dec_ref(target);
      } break;
      case INST_LOAD_TRUE:
        vec_push(stack, &true_obj);
        break;
      case INST_POP:
        obj_dec_ref(vec_pop(stack));
        break;
      case INST_POP_FRAME:
        obj_hashmap_free(vec_pop(vm->frames));
        break;
      case INST_PUSH_FRAME:
        vec_push(vm->frames, obj_hashmap_new());
        break;
      case INST_RETURN: {
        struct obj *ret = vec_pop(stack);
        vm_run_cleanup(stack);
        return ret;
      } break;
      case INST_STORE_ATTRIB: {
        struct obj *target = vec_pop(stack);
        struct obj *val = vec_peek(stack);
        obj_set_attrib(target,
                       ((struct store_attrib_inst *)inst->inner)->attrib, val);
        obj_dec_ref(target);
      }; break;
      case INST_STORE_ID: {
        struct store_id_inst *store_id = inst->inner;
        struct obj *val = vec_peek(stack);
        bool found_id = false;
        for (int i = vm->frames->len - 1; i >= 0; i--) {
          struct hashmap *frame = vec_get(vm->frames, i);
          if (obj_hashmap_has(frame, store_id->id)) {
            obj_dec_ref(obj_hashmap_get(frame, store_id->id));
            obj_hashmap_set(frame, store_id->id, obj_inc_ref(val));
            found_id = true;
            break;
          }
        }
        if (!found_id) {
          obj_hashmap_set(vec_peek(vm->frames), store_id->id, obj_inc_ref(val));
        }
      } break;
      case INST_STORE_SUBSCRIPT: {
        struct obj *target = vec_pop(stack);
        struct obj *key = vec_pop(stack);
        struct obj *val = vec_pop(stack);
        obj_store_index(target, key, val, vm);
        obj_dec_ref(key);
        obj_dec_ref(target);
        vec_push(stack, val);
      } break;
      case INST_TYPECHECK: {
        struct obj *type = vec_pop(stack);
        struct obj *target;
        struct hashmap *frame = vec_peek(vm->frames);
        if (inst->inner == NULL) {
          target = vec_peek(stack);
        } else {
          target = obj_hashmap_get(frame, inst->inner);
        }
        if (!obj_is(target, type)) {
          printf("Expected type %s, got type %s\n", (char *)type->ctx,
                 (char *)target->obj_type->ctx);
          exit(-1);
        }
        obj_dec_ref(type);
      } break;
      case INST_UNARY_OP: {
        struct obj *target = vec_pop(stack);
        switch (((struct unary_op_inst *)inst->inner)->type) {
          case UNARY_OP_NOT: {
            vec_push(stack, bool_to_obj(!obj_is_true(target, vm)));
            obj_dec_ref(target);
          } break;
          default: {
            printf("Unknown unary op!");
            exit(-1);
          }
        }
      } break;
      default:
        break;
    }

    pos++;
  }

  vm_run_cleanup(stack);

  return &none_obj;
}

static void import_attrib_from_map(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  struct hashmap *frame = usr;
  struct obj *obj_val = (struct obj *)value;
  obj_dec_ref(obj_hashmap_get(frame, key));
  obj_hashmap_set(frame, key, obj_inc_ref(obj_val));
}

static void vm_run_cleanup(struct vec *stack) { free(stack->data); }

static void vm_inst_free(struct vm_inst *);
void code_obj_free(struct code_obj *code_obj) {
  for (int i = 0; i < code_obj->instructions->len; i++) {
    vm_inst_free(vec_get(code_obj->instructions, i));
  }
  if (code_obj->name != NULL) {
    free(code_obj->name);
  }
  vec_free(code_obj->instructions);
  intmap_free(code_obj->labels);
  for (int i = 0; i < code_obj->consts->len; i++)
    obj_dec_ref(vec_get(code_obj->consts, i));
  vec_free(code_obj->consts);
  free(code_obj);
}

static void vm_inst_free(struct vm_inst *inst) {
  // printf("freeing inst %d\n", inst->type);
  switch (inst->type) {
    case INST_BUILD_CLASS:
      code_obj_free(((struct build_class_inst *)inst->inner)->code_obj);
      break;
    case INST_BUILD_FUNC: {
      struct build_func_inst *build_func = inst->inner;
      code_obj_free(build_func->code_obj);
      for (int i = 0; i < build_func->params->len; i++) {
        free(vec_get(build_func->params, i));
      }
      vec_free(build_func->params);
    } break;
    case INST_BUILD_OBJ:
      vec_free_deep(((struct build_obj_inst *)inst->inner)->keys);
      break;
    case INST_IMPORT: {
      struct import_inst *import = inst->inner;
      vec_free_deep(import->imports);
      code_obj_free(import->mod);
    } break;
    case INST_ITER_NEXT:
      free(((struct iter_next_inst *)inst->inner)->id);
      break;
    case INST_LOAD_ATTRIB:
      free(((struct load_attrib_inst *)inst->inner)->attrib);
      break;
    case INST_LOAD_ID:
      free(((struct load_id_inst *)inst->inner)->id);
      break;
    case INST_STORE_ATTRIB:
      free(((struct store_attrib_inst *)inst->inner)->attrib);
      break;
    case INST_STORE_ID:
      free(((struct store_id_inst *)inst->inner)->id);
      break;
  }
  if (inst->inner != NULL) free(inst->inner);
  free(inst);
}
