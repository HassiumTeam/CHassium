#include <vm.h>

#define STACK_PEEK() (stackptr[-1])
#define STACK_POP() (*--stackptr)
#define STACK_PUSH(v) (*stackptr++ = (v))

static void import_attrib_from_map(void *, size_t, uintptr_t, void *);
struct vm *vm_new() {
  struct vm *vm = (struct vm *)calloc(1, sizeof(struct vm));
  vm->frames = vec_new();
  vm->globals = get_defaults();
  obj_bool_init(&true_obj);
  obj_bool_init(&false_obj);
  return vm;
}

void vm_free(struct vm *vm) {
  for (int i = 0; i < vm->frames->len; i++) {
    stackframe_dec_ref(vec_get(vm->frames, i));
  }
  vec_free(vm->frames);
  obj_hashmap_free(vm->globals);
  obj_bool_free(&true_obj);
  obj_bool_free(&false_obj);
  free(vm);
}

struct obj *vm_run(struct obj *self, struct vm *vm, struct code_obj *code_obj) {
  struct obj *stack[255];
  struct obj **stackptr = stack;
  struct stackframe *topframe = vec_peek(vm->frames);
  struct obj **locals = topframe->locals;

  struct vm_inst *inst;
  int pos = 0;
  while (pos < code_obj->instructions->len) {
    inst = vec_get(code_obj->instructions, pos);
    // printf("Inst %d\n", inst->type);

    switch (inst->type) {
      case INST_BIN_OP: {
        struct obj *right = STACK_POP();
        struct obj *left = STACK_POP();
        STACK_PUSH(obj_inc_ref(obj_bin_op(
            ((struct bin_op_inst *)inst->inner)->type, left, right, vm)));
        obj_dec_ref(left);
        obj_dec_ref(right);
      } break;
      case INST_BUILD_ARRAY: {
        struct vec *items = vec_new();
        for (int i = ((struct build_array_inst *)inst->inner)->count - 1;
             i >= 0; i--)
          vec_set(items, i, obj_down_ref(STACK_POP()));
        STACK_PUSH(obj_inc_ref(obj_array_new(items)));
      } break;
      case INST_BUILD_CLASS: {
        struct build_class_inst *build_class = inst->inner;
        struct obj *class =
            obj_new(OBJ_TYPE, build_class->code_obj->name, &type_type_obj);
        obj_hashmap_set(vm->globals, build_class->code_obj->name,
                        obj_inc_ref(class));

        struct stackframe *class_frame =
            stackframe_new(build_class->code_obj->locals);
        vec_push(vm->frames, stackframe_inc_ref(class_frame));
        vm_run(self, vm, build_class->code_obj);
        vec_pop(vm->frames);

        for (int i = 0; i < class_frame->num_locals; i++) {
          struct obj *local = stackframe_get(class_frame, i);
          if (local == NULL) continue;
          if (local->type == OBJ_FUNC) {
            struct func_obj_ctx *func_ctx = (struct func_obj_ctx *)local->ctx;
            obj_set_attrib(class, func_ctx->code_obj->name, local);
          }
        }
        stackframe_dec_ref(class_frame);
      } break;
      case INST_BUILD_FUNC: {
        struct build_func_inst *build_func = inst->inner;
        struct stackframe *frame = NULL;
        if (build_func->closure) {
          frame = topframe;
        }
        STACK_PUSH(obj_inc_ref(obj_func_new(build_func->code_obj,
                                            build_func->params, NULL, frame)));
      } break;
      case INST_BUILD_OBJ: {
        struct obj *new = obj_new(OBJ_ANON, NULL, &object_type_obj);
        struct vec *keys = ((struct build_obj_inst *)inst->inner)->keys;
        for (int i = keys->len - 1; i >= 0; --i)
          obj_set_attrib(new, vec_get(keys, i), obj_down_ref(STACK_POP()));
        STACK_PUSH(obj_inc_ref(new));
      } break;
      case INST_IMPORT: {
        struct import_inst *import = inst->inner;
        struct stackframe *import_frame = stackframe_new(import->mod->locals);
        vec_push(vm->frames, stackframe_inc_ref(import_frame));
        struct obj *mod_ret = vm_run(NULL, vm, import->mod);
        vec_pop(vm->frames);
        struct hashmap *attribs = NULL;
        if (mod_ret != &none_obj) {
          attribs = mod_ret->attribs;
        }

        if (import->imports->len == 0) {
          hashmap_iterate(attribs, import_attrib_from_map,
                          vec_peek(vm->frames));
        } else {
          for (int i = 0; i < import->imports->len; i++) {
            char *attrib = vec_get(import->imports, i);
            import_attrib_from_map(attrib, 0,
                                   (uintptr_t)obj_hashmap_get(attribs, attrib),
                                   vm->globals);
          }
        }
        stackframe_dec_ref(import_frame);
        obj_dec_ref(mod_ret);
      } break;
      case INST_INVOKE: {
        int arg_count = ((struct invoke_inst *)inst->inner)->arg_count;
        struct vec *args = vec_new();
        for (int i = arg_count - 1; i >= 0; i--) vec_set(args, i, STACK_POP());
        struct obj *target = STACK_POP();
        STACK_PUSH(obj_inc_ref(obj_invoke(target, vm, args)));
        for (int i = 0; i < arg_count; i++) obj_dec_ref(vec_get(args, i));
        vec_free(args);
        obj_dec_ref(target);
      } break;
      case INST_ITER: {
        struct obj *target = STACK_POP();
        STACK_PUSH(
            obj_inc_ref(obj_invoke_attrib(target, "__iter__", vm, NULL)));
        obj_dec_ref(target);
      } break;
      case INST_JUMP:
        pos = intmap_get(code_obj->labels,
                         ((struct jump_inst *)inst->inner)->label);
        break;
      case INST_JUMP_IF_FALSE: {
        struct obj *val = STACK_POP();
        if (obj_is_false(val, vm)) {
          pos = intmap_get(code_obj->labels,
                           ((struct jump_inst *)inst->inner)->label);
        }
        obj_dec_ref(val);
      } break;
      case INST_JUMP_IF_FULL: {
        struct obj *iter = STACK_POP();
        if (obj_is_true(obj_invoke_attrib(iter, "__iterfull__", vm, NULL),
                        vm)) {
          pos = intmap_get(code_obj->labels,
                           ((struct jump_inst *)inst->inner)->label);
        } else {
          STACK_PUSH(
              obj_inc_ref(obj_invoke_attrib(iter, "__iternext__", vm, NULL)));
        }
        obj_dec_ref(iter);
      } break;
      case INST_LOAD_ATTRIB: {
        struct obj *target = STACK_POP();
        struct obj *attrib = obj_hashmap_get(
            target->attribs, ((struct load_attrib_inst *)inst->inner)->attrib);
        if (attrib == NULL) attrib = &none_obj;
        STACK_PUSH(obj_inc_ref(attrib));
        obj_dec_ref(target);
      } break;
      case INST_LOAD_CONST: {
        struct obj *const_ = vec_get(
            code_obj->consts, ((struct load_const_inst *)inst->inner)->idx);
        STACK_PUSH(obj_inc_ref(const_));
      }; break;
      case INST_LOAD_FALSE:
        STACK_PUSH(&false_obj);
        break;
      case INST_LOAD_FAST: {
        int idx = ((struct fast_inst *)inst->inner)->idx;
        STACK_PUSH(obj_inc_ref(stackframe_get(vec_peek(vm->frames), idx)));
      } break;
      case INST_LOAD_ID: {
        bool found = false;
        char *id = ((struct load_id_inst *)inst->inner)->id;
        struct obj *val;
        if (obj_hashmap_has(vm->globals, id)) {
          val = obj_hashmap_get(vm->globals, id);
        } else {
          printf("Could not load ID %s\n", id);
        }
        STACK_PUSH(obj_inc_ref(val));
      } break;
      case INST_LOAD_NONE:
        STACK_PUSH(&none_obj);
        break;
      case INST_LOAD_SELF:
        STACK_PUSH(obj_inc_ref(self));
        break;
      case INST_LOAD_SUBSCRIPT: {
        struct obj *target = STACK_POP();
        struct obj *key = STACK_POP();
        STACK_PUSH(obj_inc_ref(obj_index(target, key, vm)));
        obj_dec_ref(key);
        obj_dec_ref(target);
      } break;
      case INST_LOAD_TRUE:
        STACK_PUSH(&true_obj);
        break;
      case INST_POP:
        obj_dec_ref(STACK_POP());
        break;
      case INST_RETURN: {
        struct obj *ret = STACK_POP();
        return ret;
      } break;
      case INST_STORE_ATTRIB: {
        struct obj *target = STACK_POP();
        struct obj *val = STACK_PEEK();
        obj_set_attrib(target,
                       ((struct store_attrib_inst *)inst->inner)->attrib, val);
        obj_dec_ref(target);
      }; break;
      case INST_STORE_FAST: {
        int idx = ((struct fast_inst *)inst->inner)->idx;
        struct obj *existing = stackframe_get(vec_peek(vm->frames), idx);
        locals[idx] = obj_inc_ref(STACK_PEEK());
        obj_dec_ref(existing);
      } break;
      case INST_STORE_ID: {
        struct store_id_inst *store_id = inst->inner;
        struct obj *val = STACK_PEEK();
        struct obj *existing = obj_hashmap_get(vm->globals, store_id->id);
        obj_hashmap_set(vm->globals, store_id->id, obj_inc_ref(val));
        obj_dec_ref(existing);
      } break;
      case INST_STORE_SUBSCRIPT: {
        struct obj *target = STACK_POP();
        struct obj *key = STACK_POP();
        struct obj *val = STACK_POP();
        obj_store_index(target, key, val, vm);
        obj_dec_ref(key);
        obj_dec_ref(target);
        STACK_PUSH(val);
      } break;
      case INST_TYPECHECK: {
        struct obj *type = STACK_POP();
        struct obj *target = STACK_PEEK();
        if (!obj_is(target, type)) {
          printf("Expected type %s, got type %s\n", (char *)type->ctx,
                 (char *)target->obj_type->ctx);
          exit(-1);
        }
        obj_dec_ref(type);
      } break;
      case INST_TYPECHECK_FAST: {
        struct obj *type = STACK_POP();
        struct obj *target = locals[(uintptr_t)inst->inner];
        if (!obj_is(target, type)) {
          printf("Expected type %s, got type %s\n", (char *)type->ctx,
                 (char *)target->obj_type->ctx);
          exit(-1);
        }
        obj_dec_ref(type);
      } break;
      case INST_UNARY_OP: {
        struct obj *target = STACK_POP();
        switch (((struct unary_op_inst *)inst->inner)->type) {
          case UNARY_OP_NOT: {
            STACK_PUSH(bool_to_obj(!obj_is_true(target, vm)));
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

  return &none_obj;
}

static void import_attrib_from_map(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  struct hashmap *map = usr;
  struct obj *obj_val = (struct obj *)value;
  obj_dec_ref(obj_hashmap_get(map, key));
  obj_hashmap_set(map, key, obj_inc_ref(obj_val));
}

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
  for (int i = 0; i < code_obj->consts->len; i++) {
    obj_dec_ref(vec_get(code_obj->consts, i));
  }
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
  int type = inst->type;
  free(inst);
}
