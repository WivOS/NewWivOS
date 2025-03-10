#pragma once

#include <arch/int.h>

typedef struct {
    uint64_t ra; // 0x0
    uint64_t sp; // 0x8
    uint64_t gp; // 0x10
    uint64_t tp; // 0x18
    uint64_t t0; // 0x20
    uint64_t t1; // 0x28
    uint64_t t2; // 0x30
    uint64_t fp; // 0x38
    uint64_t s1; // 0x40
    uint64_t a0; // 0x48
    uint64_t a1; // 0x50
    uint64_t a2; // 0x58
    uint64_t a3; // 0x60
    uint64_t a4; // 0x68
    uint64_t a5; // 0x70
    uint64_t a6; // 0x78
    uint64_t a7; // 0x80
    uint64_t s2; // 0x88
    uint64_t s3; // 0x90
    uint64_t s4; // 0x98
    uint64_t s5; // 0xA0
    uint64_t s6; // 0xA8
    uint64_t s7; // 0xB0
    uint64_t s8; // 0xB8
    uint64_t s9; // 0xC0
    uint64_t s10; // 0xC8
    uint64_t s11; // 0xD0
    uint64_t t3; // 0xD8
    uint64_t t4; // 0xE0
    uint64_t t5; // 0xE8
    uint64_t t6; // 0xF0

    uint64_t epc; // 0xF8
    uint64_t cause; // 0x100
    uint64_t tval; // 0x108
    uint64_t status; // 0x110
} __attribute__((packed)) irq_regs_t;

void riscv_isr();
uintptr_t riscv_handle_trap(irq_regs_t *regs);