#include <compiler.h>
#include <vm/gc.h>
#include <vm/vm.h>

int main (int argc, char * argv[]) {
    struct has_obj  * mod;
    struct vm_state * vm;

    gc_init ();

    mod = compile ();
    vm  = vm_init (mod);

    vm_run (vm, mod, NULL);
    gc_run ();
    vm_free (vm);

    gc_run ();
    gc_free ();
}
