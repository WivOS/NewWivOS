#pragma once

#include <arch/common.h>
#include <arch/cpu.h>
#include <tasking/process.h>
#include <tasking/thread.h>

#define MAX_CPUS 256

typedef struct {
    volatile uint64_t currentCpu;
    volatile uint64_t threadUserStack;
    volatile uint64_t threadKernelStack;
    volatile uint64_t kernelStack;
    volatile uint64_t preemptCounter;
    volatile kpid_t currentPid;
    volatile ktid_t currentTid;
    volatile ktid_t currentTaskID;
} __attribute__((packed)) cpu_t;

extern cpu_t CPULocals[MAX_CPUS];

void riscv_cpu_init();
void set_cpu_struct(cpu_t *cpu);
cpu_t *get_cpu_struct();

int riscv_cpu_inited();