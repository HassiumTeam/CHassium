#include <vm.h>

struct vm *vm_new()
{
    struct vm *vm = (struct vm *)calloc(1, sizeof(struct vm));
    vm->frames = vec_new();
    return vm;
}

void vm_free(struct vm *vm)
{
    free(vm);
}

void vm_run(struct vm *vm, struct code_obj *code_obj)
{
}

struct code_obj *code_obj_new(char *name)
{
    struct code_obj *code_obj = (struct code_obj *)calloc(1, sizeof(struct code_obj));
    code_obj->name = name;
    code_obj->instructions = vec_new();
    code_obj->labels = intdict_new();
    return code_obj;
}
