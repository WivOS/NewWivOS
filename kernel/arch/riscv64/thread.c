#include <arch/thread.h>

#include <arch/riscv64/cpu.h>

extern uint64_t get_kernel_gp();

void arch_thread_init_regs(thread_t *thread, thread_parameter_types_t dataType, void *data) {
    thread->saved_regs.gp = get_kernel_gp();
    thread->saved_regs.status |= (1 << 8) | (1 << 5);
    switch(dataType) {
        case thread_parameter_entry:
            {
                thread_parameter_entry_data_t *entryData = (thread_parameter_entry_data_t *)data;
                thread->saved_regs.epc = (uint64_t)entryData->entry;
                thread->saved_regs.a0 = (uint64_t)entryData->arg;
            }
            break;
    }
}

void arch_thread_set_stack(thread_t *thread, void *stack) {
    thread->saved_regs.sp = (uint64_t)stack;
}