#pragma once

#include <arch/common.h>
#include <arch/cpu.h>
#include <tasking/process.h>
#include <tasking/thread.h>

#define MAX_CPUS 256

#define IPI_RESCHEDULE 1

#define CPU_FLAG_TIMER (1 << 0)

#define RISCV_FEATURE_ATOMIC       (1 << 0)
#define RISCV_FEATURE_COMPRESSED   (1 << 2)
#define RISCV_FEATURE_DOUBLE_FP    (1 << 3)
#define RISCV_FEATURE_RV32E        (1 << 4)
#define RISCV_FEATURE_SINGLE_FP    (1 << 5)
#define RISCV_FEATURE_HYPERVISOR   (1 << 7)
#define RISCV_FEATURE_RV_INTEGER   (1 << 8)
#define RISCV_FEATURE_INT_MULT_DIV (1 << 12)
#define RISCV_FEATURE_QUAD_FP      (1 << 16)
#define RISCV_FEATURE_SUPERVISOR   (1 << 18)
#define RISCV_FEATURE_USER         (1 << 20)
#define RISCV_FEATURE_NONSTANDARD  (1 << 23)

typedef struct {
    volatile uint64_t currentCpu;
    volatile uint64_t threadUserStack;
    volatile uint64_t threadKernelStack;
    volatile uint64_t kernelStack;
    volatile uint64_t preemptCounter;
    volatile kpid_t currentPid;
    volatile ktid_t currentTid;
    volatile ktid_t currentTaskID;
    volatile uint8_t ipiType;
    volatile uint8_t flags;
    volatile uint32_t isaFeatures;
} __attribute__((packed)) cpu_t;

extern volatile cpu_t CPULocals[MAX_CPUS];

void riscv_cpu_init();
void set_cpu_struct(cpu_t *cpu);
cpu_t *get_cpu_struct();

int riscv_cpu_inited();