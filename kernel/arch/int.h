#pragma once

#include <arch/common.h>

void arch_interrupt_init();
void arch_interrupt_init_smp();

uint32_t arch_interrupt_max();
uint32_t arch_interrupt_min();

int arch_interrupts_enabled();
void arch_enable_interrupts();
int arch_disable_interrupts();

void arch_interrupt_enable(uint32_t id);
void arch_interrupt_disable(uint32_t id);
void arch_interrupt_complete(uint32_t id);