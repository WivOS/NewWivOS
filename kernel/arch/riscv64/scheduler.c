#include <arch/scheduler.h>
#include <tasking/scheduler.h>

#include <arch/cpu.h>
#include <arch/riscv64/cpu.h>

#include <utils/system.h>

int printf_scheduler(const char *format, ...);

__attribute__((noinline)) static void arch_scheduler_idle_halt() {
    //printf_scheduler("[%d][Scheduler] Idling\n", arch_cpu_get_index());

    arch_cpu_set_current_taskid(-1);
    arch_cpu_set_current_pid(-1);
    arch_cpu_set_current_tid(-1);

    spinlock_unlock_skippreempt(&SchedulerLock);
    spinlock_unlock_skippreempt(&EntrySchedulerLock);
    arch_enable_interrupts();
    while(1) {
        asm volatile ("wfi;");
    }
}

__attribute__((noinline)) void arch_scheduler_idle() {
    vmm_set_pt(KernelPageMap);
    asm volatile (
        "ld sp, 24(tp);"
        "j arch_scheduler_idle_halt;"
        ::: "memory"
    );
    arch_scheduler_idle_halt();
}

uint64_t riscv_vmm_create_satp(void *entries);
__attribute__((noreturn)) void arch_scheduler_context_switch(volatile irq_regs_t *regs, volatile uintptr_t entries);

#define SIE_STIE (1 << 5)
__attribute__((noreturn)) void arch_scheduler_switch_task(volatile irq_regs_t *regs, volatile void *page_table) {
    //printf_scheduler("[%d] TODO: Scheduler Switch Task\n", arch_cpu_get_index());
    regs->tp = (uint64_t)get_cpu_struct();

    if(page_table != NULL) {
        uint64_t satp = riscv_vmm_create_satp((void *)page_table);
        arch_scheduler_context_switch(regs, satp);
    } else {
        arch_scheduler_context_switch(regs, 0);
    }
}

void arch_scheduler_unlock_locks() {
    spinlock_unlock_skippreempt(&SchedulerLock);
    spinlock_unlock_skippreempt(&EntrySchedulerLock);
}