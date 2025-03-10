#pragma once

#include <arch/common.h>

void arch_interrupt_init();

uint32_t arch_interrupt_max();
uint32_t arch_interrupt_min();

void arch_interrupt_enable(uint32_t id);
void arch_interrupt_disable(uint32_t id);
void arch_interrupt_complete(uint32_t id);