#include <arch/x86_64/arch.h>

void arch_init() {
    //TODO

    kmain();
    while(1);
}

void arch_post_vmm() {

}

void arch_debug_printf_function(debug_printf_function_t *func, char c) {
    if(!func->size || func->size < func->currentPos) {
        outb(0xE9, c);
        func->currentPos++;
    }
}