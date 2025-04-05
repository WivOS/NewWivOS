#include <tasking/thread.h>
#include <tasking/process.h>
#include <arch/thread.h>

#include <mem/alloc.h>
#include <mem/pmm.h>
#include <mem/vmm.h>

#include <utils/string.h>
#include <utils/system.h>

#define STACK_LOCATION_TOP ((size_t)0x0000800000000000)

#define STACK_SIZE (4 * PAGE_SIZE)
#define STACK_GUARD_SIZE (PAGE_SIZE)
#define KSTACK_SIZE (8 * PAGE_SIZE)

ktid_t thread_create(kpid_t pid, thread_parameter_types_t dataType, void *data) {
    process_t *process = process_get_process(pid);
    if(process == NULL) return -1;

    ktid_t tid = 0;
    for(; tid < MAX_THREADS; tid++) {
        if(process->threads[tid] == NULL || process->threads[tid] == (void *)-1) {
            goto done;
        }
    }
    printf("Max thread number allocated reached\n");
    spinlock_unlock(&process->lock);
    return -1;

done: (void)tid;

    thread_t *newThread = kmalloc(sizeof(thread_t));
    newThread->lock = INIT_SPINLOCK();
    newThread->pid = pid;
    newThread->tid = tid;
    newThread->taskID = -1;
    arch_thread_init_regs(newThread, dataType, data);

    newThread->kstack_address = (void *)((uint64_t)kmalloc(KSTACK_SIZE) + KSTACK_SIZE);
    *((size_t *)((uint64_t)newThread->kstack_address - sizeof(size_t))) = 0;

    void *stack;
    if(!pid) {
        stack = (void *)newThread->kstack_address;
    } else {
        size_t stackGuardPage = STACK_LOCATION_TOP - (STACK_SIZE + STACK_GUARD_SIZE) * (tid + 1);
        size_t stackBottom = stackGuardPage + STACK_GUARD_SIZE;

        void *memoryStack = (void *)pmm_alloc(STACK_SIZE / PAGE_SIZE);
        size_t *sbase = (size_t *)(memoryStack + STACK_SIZE + MEM_PHYS_OFFSET);

        size_t *sp;
        switch(dataType) {
            case thread_parameter_entry:
                sp = sbase;
                break;
        }

        vmm_map((pt_t *)process->page_table, (void *)stackBottom, memoryStack, STACK_SIZE / PAGE_SIZE, VMM_EXECUTE | VMM_READ | VMM_WRITE | VMM_USER);
        vmm_unmap((pt_t *)process->page_table, (void *)stackGuardPage, STACK_GUARD_SIZE / PAGE_SIZE);

        stack = (void *)(stackBottom + STACK_SIZE - ((sbase - sp) * sizeof(size_t)));
    }
    printf("%llX\n", stack);
    arch_thread_set_stack(newThread, stack);

    process->threads[tid] = newThread;

    spinlock_unlock(&process->lock);

    return tid;
}