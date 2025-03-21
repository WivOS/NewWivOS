#pragma once

#include <arch/defines.h>

void arch_scheduler_idle();
void arch_scheduler_switch_task(volatile irq_regs_t *regs, volatile void *page_table);