#include <vm.h>

static struct obj *handle_bin_op(struct vm *, bin_op_type_t, struct obj *, struct obj *);

struct vm *vm_new()
{
    struct vm *vm = (struct vm *)calloc(1, sizeof(struct vm));
    vm->frames = vec_new();
    vec_push(vm->frames, obj_hashmap_new());
    return vm;
}

void vm_free(struct vm *vm)
{
    for (int i = 0; i < vm->frames->len; i++)
    {

        struct hashmap *map = vec_get(vm->frames, i);
        size_t iter = 0;
        void *item;
        while (hashmap_iter(map, &iter, &item))
        {
            struct obj_hashmap_entry *entry = item;
            obj_dec_ref(entry->obj);
        }
        hashmap_free(map);
    }
    vec_free(vm->frames);
    free(vm);
}

void vm_run(struct vm *vm, struct code_obj *code_obj)
{
    struct vec *stack = vec_new();
    struct vm_inst *inst;
    int pos = 0;

    while (pos < code_obj->instructions->len)
    {
        inst = vec_get(code_obj->instructions, pos);
        printf("Inst %d\n", inst->type);

        switch (inst->type)
        {
        case INST_LOAD_NUM:
            vec_push(stack, obj_inc_ref(obj_num_new(
                                ((struct load_num_inst *)inst->inner)->value)));
            break;
        case INST_POP:
            obj_dec_ref(vec_pop(stack));
        default:
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
        // printf("freeing inst %d\n", ((struct vm_inst *)vec_get(code_obj->instructions, i))->type);
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
