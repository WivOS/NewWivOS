#pragma once

#include <arch/common.h>
#include <arch/cpu.h>
#include <tasking/process.h>
#include <tasking/thread.h>

#define MAX_CPUS 256

#define IPI_RESCHEDULE 1

#define CPU_FLAG_TIMER (1 << 0)

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
} __attribute__((packed)) cpu_t;

extern volatile cpu_t CPULocals[MAX_CPUS];

void riscv_cpu_init();
void set_cpu_struct(cpu_t *cpu);
cpu_t *get_cpu_struct();

int riscv_cpu_inited();