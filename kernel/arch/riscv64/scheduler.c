#include <arch/scheduler.h>

//TODO
void arch_scheduler_idle() {
    arch_disable_interrupts();
    while(1);
}

//TODO
void arch_scheduler_switch_task(volatile irq_regs_t *regs, volatile void *page_table) {
    arch_disable_interrupts();
    while(1);
}