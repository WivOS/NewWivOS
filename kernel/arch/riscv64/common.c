#include <arch/riscv64/arch.h>
#include <arch/riscv64/cpu.h>
#include <arch/int.h>
#include <boot/limine.h>
#include <utils/dtb.h>

void arch_init() {
    kmain();
    while(1);
}

void arch_post_vmm() {
    void *dtb_addr = riscv64_get_boot_dtb();
    dtb_init((uintptr_t)dtb_addr);

    plic_init();

    riscv_cpu_init();
    arch_interrupt_init();
}

//TODO: This is not true
static unsigned char * debug_uart = (unsigned char *)0x10000000;
void arch_debug_printf_function(debug_printf_function_t *func, char c) {
    if(!func->size || func->size < func->currentPos) {
        while(!(*((volatile uint8_t *)&debug_uart[5]) & (1 << 5)));
        *((volatile uint8_t *)debug_uart) = c;
        func->currentPos++;
    }
}