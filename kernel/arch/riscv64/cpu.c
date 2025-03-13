#include <arch/common.h>
#include <boot/limine.h>
#include <arch/riscv64/cpu.h>
#include <arch/riscv64/defines.h>

#include <utils/system.h>

cpu_t CPULocals[MAX_CPUS];

#define CPU_STACK_SIZE 16384
struct stack_t {
    uint8_t guardPage[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
    uint8_t stack[CPU_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
};
static struct stack_t CPUStacks[MAX_CPUS] __attribute__((aligned(PAGE_SIZE)));


static int cpu_count = 0;
static void init_cpu(uint64_t hartid) {
    //TODO: Mix processor and hartid
    CPULocals[cpu_count].currentCpu = hartid;
    CPULocals[cpu_count].kernelStack = (volatile size_t)&CPUStacks[cpu_count].stack[CPU_STACK_SIZE];
    CPULocals[cpu_count].preemptCounter = 0;
    //TODO: User stack

    cpu_count++;
}

static int cpu_is_inited = 0;
int riscv_cpu_inited() {
    return cpu_is_inited;
}

void riscv_cpu_init() {
    limine_mp_riscv64_response_t *mp_response = (limine_mp_riscv64_response_t *)limine_get_mp();
    if(mp_response == NULL) {
        printf("CPU info not provided by limine\n");
        while(1);
    }

    init_cpu(mp_response->bsp_hartid);
    set_cpu_struct(&CPULocals[0]);

    for(int i = 0; i < mp_response->cpu_count; i++) {
        limine_mp_riscv64_info_t *mp_info = mp_response->cpus[i];
        if(mp_info->hartid == mp_response->bsp_hartid) continue;

        init_cpu(mp_info->hartid);
    }

    cpu_is_inited = 1;
}