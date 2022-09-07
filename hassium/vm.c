#include <sys/time.h>
#include <vm.h>

// static long times[34];
// static long counts[34];

#define opcode ((uint16_t)((inst & 0xFFFF000000000000) >> 48))
#define opshort ((uint16_t)((inst & 0x0000FFFF00000000) >> 32))
#define op ((uint32_t)(inst & 0x00000000FFFFFFFF))

#define STACK_PEEK() (stackptr[-1])
#define STACK_POP() (*--stackptr)
#define STACK_PUSH(v) (*stackptr++ = (v))

static void extend_attrib_from_map(void *, size_t, uintptr_t, void *);
static void import_attrib_from_map(void *, size_t, uintptr_t, void *);

struct vm *vm_new() {
  struct vm *vm = calloc(1, sizeof(struct vm));
  vm->frames = vec_new();
  vm->globals = get_defaults();
  vm->handlers = vec_new();
  vm->handler_returns = vec_new();
  // memset(&times, 0, sizeof(times));
  // memset(&times, 0, sizeof(counts));
  return vm;
}

void vm_free(struct vm *vm) {
  for (int i = 0; i < vm->frames->len; ++i) {
    stackframe_dec_ref(vec_get(vm->frames, i));
  }
  vec_free(vm->handlers);
  vec_free(vm->handler_returns);
  vec_free(vm->frames);
  obj_hashmap_free(vm->globals);
  destruct_defaults();
  free(vm);
}

// long getMicrotime() {
//   struct timeval currentTime;
//   gettimeofday(&currentTime, NULL);
//   return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
// }

void debug() {
  // for (int i = 0; i < 34; ++i) {
  //   if (counts[i] == 0) continue;
  //   printf("Opcode: %d ran %ld times, avg: %f, total: %ld\n", i, counts[i],
  //          (float)times[i] / (float)counts[i], times[i]);
  // }
}

struct obj *vm_run(struct vm *vm, struct code_obj *code_obj, struct obj *self) {
  struct obj *stack[255];
  struct obj **stackptr = stack;
  struct stackframe *topframe = vec_peek(vm->frames);
  struct obj **locals = topframe->locals;
  int *handler_returns_len = &vm->handler_returns->len;
  struct vm_inst *inst;

  int pos = 0;
  code_obj->pos = &pos;

  for (;;) {
    if (*handler_returns_len > 0) {
      struct obj *handler_return = vec_peek(vm->handler_returns);
      struct code_obj *handler_code_obj =
          ((struct func_obj_ctx *)handler_return->ctx)->code_obj;
      struct code_obj *parent_code_obj = handler_code_obj->parent;

      if (parent_code_obj == code_obj) {
        pos = intmap_get(code_obj->labels, handler_code_obj->caught_label) + 1;
        obj_dec_ref(vec_pop(vm->handler_returns));
      } else {
        return &none_obj;
      }
    }

    vm_inst_t inst = (vm_inst_t)vec_get(code_obj->instructions, pos);

    // printf("%d %d %d\n", opcode, opshort, op);

    // long startTime = getMicrotime();
    // ++counts[opcode];

    switch (opcode) {
      case INST_BIN_OP: {
        struct obj *right = STACK_POP();
        struct obj *left = STACK_POP();

        STACK_PUSH(obj_inc_ref(obj_bin_op(opshort, left, right, vm)));

        obj_dec_ref(left);
        obj_dec_ref(right);
      } break;
      case INST_BUILD_ARRAY: {
        struct vec *items = vec_new();
        for (int i = op - 1; i >= 0; i--) {
          vec_set(items, i, obj_down_ref(STACK_POP()));
        }

        STACK_PUSH(obj_inc_ref(obj_array_new(items)));
      } break;
      case INST_BUILD_CLASS: {
        struct code_obj *class_code_obj = vec_get(code_obj->code_objs, op);
        struct obj *class =
            obj_new(OBJ_TYPE, class_code_obj->name, &type_type_obj);
        struct stackframe *class_frame =
            stackframe_new(class_code_obj->locals, class);

        obj_hashmap_set(vm->globals, class_code_obj->name, obj_inc_ref(class));
        vec_push(vm->frames, stackframe_inc_ref(class_frame));
        vm_run(vm, class_code_obj, self);
        vec_pop(vm->frames);

        if (opshort) {
          struct obj *extends = STACK_POP();
          hashmap_iterate(extends->attribs, extend_attrib_from_map,
                          class->attribs);
          class->parent = extends;
        }

        for (int i = 0; i < class_frame->num_locals; ++i) {
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
        struct stackframe *frame = NULL;
        if (opshort) {
          frame = topframe;
        }

        struct code_obj *func_code_obj = vec_get(code_obj->code_objs, op);
        struct obj *func_obj = obj_func_new(
            func_code_obj, func_code_obj->params, NULL, frame, true);
        func_obj->sourcepos = sourcepos_inc_ref(
            (struct sourcepos *)vec_get(code_obj->sourceposes, pos));
        STACK_PUSH(obj_inc_ref(func_obj));
      } break;
      case INST_BUILD_HANDLER: {
        struct code_obj *handler = vec_get(code_obj->code_objs, op);
        vec_push(vm->handlers,
                 obj_func_new(handler, handler->params, NULL, topframe, true));
      } break;
      case INST_BUILD_OBJ: {
        struct obj *new = obj_new(OBJ_ANON, NULL, &object_type_obj);
        struct vec *keys = vec_get(code_obj->vecs, op);

        for (int i = keys->len - 1; i >= 0; --i) {
          obj_set_attrib(new, vec_get(keys, i), obj_down_ref(STACK_POP()));
        }

        STACK_PUSH(obj_inc_ref(new));
      } break;
      case INST_DELETE: {
        STACK_POP()->refs = 1;
      } break;
      case INST_IMPORT: {
        struct code_obj *mod = vec_get(code_obj->code_objs, op);
        struct vec *imports = vec_get(code_obj->vecs, opshort);
        struct stackframe *import_frame = stackframe_new(mod->locals, NULL);

        vec_push(vm->frames, stackframe_inc_ref(import_frame));
        struct obj *mod_ret = vm_run(vm, mod, NULL);
        vec_pop(vm->frames);

        struct hashmap *attribs = NULL;
        if (mod_ret != &none_obj) {
          attribs = mod_ret->attribs;
        }

        if (imports->len == 0) {
          hashmap_iterate(attribs, import_attrib_from_map, vm->globals);
        } else {
          for (int i = 0; i < imports->len; ++i) {
            char *attrib = vec_get(imports, i);
            import_attrib_from_map(attrib, 0,
                                   (uintptr_t)obj_hashmap_get(attribs, attrib),
                                   vm->globals);
          }
        }

        stackframe_dec_ref(import_frame);
        obj_dec_ref(mod_ret);
      } break;
      case INST_INVOKE: {
        struct vec args;
        args.data = (void **)(stackptr -= op);
        args.len = op;

        struct obj *target = STACK_POP();
        // long inbetweenTimeStart = getMicrotime();
        STACK_PUSH(obj_inc_ref(obj_invoke(target, vm, &args)));
        // long inbetweenTimeEnd = getMicrotime();

        for (int i = 0; i < op; ++i) {
          obj_dec_ref(vec_get(&args, i));
        }
        obj_dec_ref(target);
        // times[opcode] += getMicrotime() - startTime -
        //                  (inbetweenTimeEnd - inbetweenTimeStart);
      } break;
      case INST_ITER: {
        struct obj *target = STACK_POP();
        if (obj_hashmap_has(target->attribs, "__iter__")) {
          STACK_PUSH(
              obj_inc_ref(obj_invoke_attrib(target, "__iter__", vm, NULL)));
        } else if (target->ops && target->ops->__iter__) {
          STACK_PUSH(obj_inc_ref(target->ops->__iter__(target, vm, NULL)));
        } else {
          printf("Could not use __iter__ on %d\n", target->type);
          exit(-1);
        }

        obj_dec_ref(target);
      } break;
      case INST_JUMP:
        pos = intmap_get(code_obj->labels, op);
        break;
      case INST_JUMP_IF_FALSE: {
        struct obj *val = STACK_POP();
        if (!obj_is_true(val, vm)) {
          pos = intmap_get(code_obj->labels, op);
        }
        obj_dec_ref(val);
      } break;
      case INST_JUMP_IF_FULL: {
        struct obj *iter = STACK_POP();
        if (obj_is_true(obj_invoke_attrib(iter, "__iterfull__", vm, NULL),
                        vm)) {
          pos = intmap_get(code_obj->labels, op);
        } else {
          STACK_PUSH(
              obj_inc_ref(obj_invoke_attrib(iter, "__iternext__", vm, NULL)));
        }
        obj_dec_ref(iter);
      } break;
      case INST_LOAD_ATTRIB: {
        struct obj *target = STACK_POP();

        if (target == &none_obj) {
          vm_raise(vm,
                   obj_no_such_attrib_error_new(
                       target, obj_string_new(vec_get(code_obj->strs, op))));
          STACK_PUSH(&none_obj);
          break;
        }

        if (target->lazy_load_fn) {
          target->lazy_load_fn(target);
          target->lazy_load_fn = NULL;
        }

        struct obj *attrib =
            obj_hashmap_get(target->attribs, vec_get(code_obj->strs, op));
        if (attrib == NULL) {
          attrib = &none_obj;
        }

        STACK_PUSH(obj_inc_ref(attrib));

        obj_dec_ref(target);
      } break;
      case INST_LOAD_CONST:
        STACK_PUSH(obj_inc_ref(vec_get(code_obj->consts, op)));
        break;
      case INST_LOAD_FALSE:
        STACK_PUSH(&false_obj);
        break;
      case INST_LOAD_FAST:
        STACK_PUSH(obj_inc_ref(stackframe_get(topframe, op)));
        break;
      case INST_LOAD_ID: {
        char *id = vec_get(code_obj->strs, op);
        struct obj *val;
        if (obj_hashmap_has_get(vm->globals, id, &val)) {
          STACK_PUSH(obj_inc_ref(val));
        } else {
          vm_raise(vm, obj_name_error_new(obj_string_new(id)));
        }
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

        if (target->lazy_load_fn) {
          target->lazy_load_fn(target);
          target->lazy_load_fn = NULL;
        }

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
      case INST_POP_HANDLER:
        obj_dec_ref(vec_pop(vm->handlers));
        break;
      case INST_RAISE:
        vm_raise(vm, obj_down_ref(STACK_POP()));
        break;
      case INST_RETURN:
        return STACK_POP();
        break;
      case INST_SLICE: {
        struct obj *end = STACK_POP();
        struct obj *start = STACK_POP();
        struct obj *target = STACK_POP();

        struct obj *argstack[2];
        argstack[0] = start;
        argstack[1] = end;
        struct vec args;
        args.data = (void **)argstack;
        args.len = 2;

        if (target->lazy_load_fn) {
          target->lazy_load_fn(target);
          target->lazy_load_fn = NULL;
        }

        STACK_PUSH(
            obj_inc_ref(obj_invoke_attrib(target, "__slice__", vm, &args)));

        obj_dec_ref(start);
        obj_dec_ref(end);
        obj_dec_ref(target);
      } break;
      case INST_STORE_ATTRIB: {
        struct obj *target = STACK_POP();
        struct obj *val = STACK_PEEK();

        if (target->lazy_load_fn) {
          target->lazy_load_fn(target);
          target->lazy_load_fn = NULL;
        }

        obj_set_attrib(target, vec_get(code_obj->strs, op), val);
        obj_dec_ref(target);
      }; break;
      case INST_STORE_FAST: {
        struct obj *existing =
            stackframe_get((struct stackframe *)vec_peek(vm->frames), op);
        locals[op] = obj_inc_ref(STACK_PEEK());
        obj_dec_ref(existing);
      } break;
      case INST_STORE_ID: {
        char *id = vec_get(code_obj->strs, op);
        struct obj *val = STACK_PEEK();
        struct obj *existing = obj_hashmap_get(vm->globals, id);

        obj_hashmap_set(vm->globals, id, obj_inc_ref(val));
        obj_dec_ref(existing);
      } break;
      case INST_STORE_SUBSCRIPT: {
        struct obj *target = STACK_POP();
        struct obj *key = STACK_POP();
        struct obj *val = STACK_POP();

        if (target->lazy_load_fn) {
          target->lazy_load_fn(target);
          target->lazy_load_fn = NULL;
        }

        obj_store_index(target, key, val, vm);
        obj_dec_ref(key);
        obj_dec_ref(target);
        STACK_PUSH(val);
      } break;
      case INST_TYPECHECK: {
        struct obj *type = STACK_POP();
        struct obj *target = STACK_PEEK();

        if (!obj_is(target, type)) {
          vm_raise(vm, obj_type_error_new(target, type, target->obj_type));
        }

        obj_dec_ref(type);
      } break;
      case INST_TYPECHECK_FAST: {
        struct obj *type = STACK_POP();
        struct obj *target = locals[op];

        if (!obj_is(target, type)) {
          vm_raise(vm, obj_type_error_new(target, type, target->obj_type));
        }

        obj_dec_ref(type);
      } break;
      case INST_UNARY_OP: {
        struct obj *target = STACK_POP();
        switch (opshort) {
          case UNARY_OP_NOT:
            STACK_PUSH(bool_to_obj(!obj_is_true(target, vm)));
            obj_dec_ref(target);
            break;
          case UNARY_OP_POST_DEC:
          case UNARY_OP_POST_INC:
            obj_dec_ref(target);
            break;
          default:
            printf("Bad unary op, something has gone terribly wrong....\n");
            exit(-1);
            break;
        }
      } break;
      default:
        break;
    }

    // if (opcode != INST_INVOKE) {
    //   times[opcode] += getMicrotime() - startTime;
    // }

    ++pos;
  }

  return &none_obj;
}

void vm_raise(struct vm *vm, struct obj *obj) {
  struct strbuf *stacktrace = strbuf_new();
  bool has_some_trace = false;
  bool found_top = false;
  struct sourcepos *curpos;

  if (obj->obj_type == &compile_error_type_obj) {
    strbuf_append_str(stacktrace, "at: ");
    sourcepos_to_strbuf(obj->sourcepos, stacktrace);
    strbuf_append_str(stacktrace, ":\n");
    strbuf_append_str(stacktrace,
                      (char *)vec_get(obj->sourcepos->sourcefile->lines,
                                      obj->sourcepos->row));
    strbuf_append(stacktrace, '\n');
    for (int i = 0; i < obj->sourcepos->col; ++i) {
      strbuf_append(stacktrace, ' ');
    }
    strbuf_append_str(stacktrace, "^\n");
    has_some_trace = true;
  }

  for (int i = vm->frames->len - 1; i >= 0; --i) {
    struct stackframe *frame = vec_get(vm->frames, i);
    if (frame->invokee != NULL) {
      if (frame->invokee->type == OBJ_FUNC && !found_top) {
        struct func_obj_ctx *func_ctx = frame->invokee->ctx;
        curpos =
            vec_get(func_ctx->code_obj->sourceposes, *func_ctx->code_obj->pos);
        strbuf_append_str(stacktrace, "in: ");
        sourcepos_to_strbuf(curpos, stacktrace);
        strbuf_append_str(stacktrace, ":\n");
        strbuf_append_str(stacktrace, (char *)vec_get(curpos->sourcefile->lines,
                                                      curpos->row));
        strbuf_append(stacktrace, '\n');
        for (int i = 0; i < curpos->col; ++i) {
          strbuf_append(stacktrace, ' ');
        }
        strbuf_append_str(stacktrace, "^\n");
        found_top = true;
      }

      strbuf_append_str(stacktrace, "  at ");

      struct obj *toString_res = obj_to_string(frame->invokee, vm);
      strbuf_append_str(stacktrace, (char *)toString_res->ctx);
      obj_dec_ref(toString_res);

      if (frame->invokee != NULL && frame->invokee->type == OBJ_FUNC) {
        struct func_obj_ctx *func_ctx = frame->invokee->ctx;
        strbuf_append(stacktrace, '(');
        sourcepos_to_strbuf(
            vec_get(func_ctx->code_obj->sourceposes, *func_ctx->code_obj->pos),
            stacktrace);
        strbuf_append(stacktrace, ')');
      }
      strbuf_append(stacktrace, '\n');

      has_some_trace = true;
    }
  }

  char *trace = strbuf_done(stacktrace);
  if (has_some_trace) {
    obj_set_attrib(obj, "trace", obj_string_new(trace));
  }
  free(trace);

  if (vm->handlers->len == 0) {
    printf("Unhandled error:\n%s\n", (char *)obj_to_string(obj, vm)->ctx);
    if (has_some_trace) {
      printf("%s", (char *)obj_hashmap_get(obj->attribs, "trace")->ctx);
    }
    exit(-1);
  }

  struct obj *handler = vec_pop(vm->handlers);
  struct func_obj_ctx *handler_ctx = handler->ctx;

  struct vec args;
  struct obj *argstack[1];
  args.data = (void **)argstack;
  args.data[0] = obj;
  args.len = handler_ctx->params->len;

  obj_invoke(handler, vm, &args);
  vec_push(vm->handler_returns, handler);
}

static void import_attrib_from_map(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  struct hashmap *map = usr;
  struct obj *obj_val = (struct obj *)value;
  struct obj *existing = obj_hashmap_get(map, key);
  obj_hashmap_set(map, key, obj_inc_ref(obj_val));
  obj_dec_ref(existing);
}

static void extend_attrib_from_map(void *key, size_t ksize, uintptr_t value,
                                   void *usr) {
  struct hashmap *map = usr;
  struct obj *obj_val = (struct obj *)value;
  struct obj *existing = obj_hashmap_get(map, key);
  if (strcmp(key, "new") == 0) {
    obj_hashmap_set(map, "__super__", obj_inc_ref(obj_val));
  } else {
    obj_hashmap_set(map, key, obj_inc_ref(obj_val));
  }
  obj_dec_ref(existing);
}