#include <vm.h>

struct vm *vm_new()
{
    struct vm *vm = (struct vm *)calloc(1, sizeof(struct vm));
    vm->frames = vec_new();
    return vm;
}

void vm_free(struct vm *vm)
{
    vec_free(vm->frames);
    free(vm);
}

void vm_run(struct vm *vm, struct code_obj *code_obj)
{
    vec_push(vm->frames, get_defaults());
    struct vec *stack = vec_new();

    struct vm_inst *inst;
    struct obj *obj;
    int arg_count = 0;
    int pos = 0;
    while (pos < code_obj->instructions->len)
    {
        inst = vec_get(code_obj->instructions, pos);
        printf("Inst %d\n", inst->type);

        switch (inst->type)
        {
        case INST_INVOKE:
            arg_count = ((struct invoke_inst *)inst->inner)->arg_count;
            struct vec *args = vec_new();
            for (int i = 0; i < arg_count; i++)
                vec_push(args, vec_pop(stack));
            obj = vec_pop(stack);
            obj_invoke(obj, vm, args);
            break;
        case INST_LOAD_ID:
            for (int i = 0; i < vm->frames->len; i++)
            {
                if ((obj = obj_hashmap_get(vec_get(vm->frames, i), ((struct load_id_inst *)inst->inner)->id)))
                {
                    vec_push(stack, obj);
                }
            }
            break;
        case INST_LOAD_NUM:
            vec_push(stack, num_obj_new(((struct load_num_inst *)inst->inner)->value));
            break;
        case INST_LOAD_STR:
            vec_push(stack, str_obj_new(clone_str(((struct load_str_inst *)inst->inner)->str)));
            break;
        }
        pos++;
    }
    vec_free(stack);
}

struct code_obj *code_obj_new(char *name)
{
    struct code_obj *code_obj = (struct code_obj *)calloc(1, sizeof(struct code_obj));
    code_obj->name = name;
    code_obj->instructions = vec_new();
    code_obj->labels = intmap_new();
    return code_obj;
}

static void vm_inst_free(struct vm_inst *);
void code_obj_free(struct code_obj *code_obj)
{
    for (int i = 0; i < code_obj->instructions->len; i++)
    {
        printf("inst %d\n", ((struct vm_inst *)vec_get(code_obj->instructions, i))->type);
        vm_inst_free(vec_get(code_obj->instructions, i));
    }
    vec_free(code_obj->instructions);
    intmap_free(code_obj->labels);
    free(code_obj);
}

static void vm_inst_free(struct vm_inst *inst)
{
    switch (inst->type)
    {
    case INST_BUILD_CLASS:
        code_obj_free(((struct build_class_inst *)inst->inner)->code_obj);
        break;
    case INST_BUILD_FUNC:
        code_obj_free(((struct build_func_inst *)inst->inner)->code_obj);
        vec_free_deep(((struct build_func_inst *)inst->inner)->params);
        break;
    case INST_LOAD_ATTRIB:
        free(((struct load_attrib_inst *)inst->inner)->attrib);
        break;
    case INST_LOAD_ID:
        free(((struct load_id_inst *)inst->inner)->id);
        break;
    case INST_LOAD_STR:
        free(((struct load_str_inst *)inst->inner)->str);
        break;
    case INST_STORE_ATTRIB:
        free(((struct store_attrib_inst *)inst->inner)->attrib);
        break;
    case INST_STORE_ID:
        free(((struct store_id_inst *)inst->inner)->id);
        break;
    }
    if (inst->inner != NULL)
        free(inst->inner);
    free(inst);
}
