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

size_t arch_cpu_get_index() {
    if(riscv_cpu_inited()) return get_cpu_struct()->currentCpu;
    else return -1;
}

void arch_cpu_increment_preempt_counter() {
    if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter++;
}

void arch_cpu_decrement_preempt_counter() {
    if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter--;
}

size_t arch_cpu_get_preempt_counter() {
    if(riscv_cpu_inited()) return get_cpu_struct()->preemptCounter;
    else return 0;
}

kpid_t arch_cpu_get_current_pid() {
    if(riscv_cpu_inited()) return get_cpu_struct()->currentPid;
    else return -1;
}

ktid_t arch_cpu_get_current_tid() {
    if(riscv_cpu_inited()) return get_cpu_struct()->currentTid;
    else return -1;
}

ktid_t arch_cpu_get_current_taskid() {
    if(riscv_cpu_inited()) return get_cpu_struct()->currentTaskID;
    else return -1;
}

void *arch_cpu_get_thread_user_stack() {
    if(riscv_cpu_inited()) return (void *)get_cpu_struct()->threadUserStack;
    else return NULL;
}

void *arch_cpu_get_thread_kernel_stack() {
    if(riscv_cpu_inited()) return (void *)get_cpu_struct()->threadKernelStack;
    else return NULL;
}

void arch_cpu_set_current_pid(kpid_t kpid) {
    if(riscv_cpu_inited()) get_cpu_struct()->currentPid = kpid;
}

void arch_cpu_set_current_tid(ktid_t ktid) {
    if(riscv_cpu_inited()) get_cpu_struct()->currentTid = ktid;
}

void arch_cpu_set_current_taskid(ktid_t taskID) {
    if(riscv_cpu_inited()) get_cpu_struct()->currentTaskID = taskID;
}

void arch_cpu_set_thread_user_stack(void *address) {
    if(riscv_cpu_inited()) get_cpu_struct()->threadUserStack = (uint64_t)address;
}

void arch_cpu_set_thread_kernel_stack(void *address) {
    if(riscv_cpu_inited()) get_cpu_struct()->threadKernelStack = (uint64_t)address;
}
