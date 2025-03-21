#include <tasking/process.h>
#include <tasking/thread.h>

#include <mem/alloc.h>
#include <arch/cpu.h>
#include <arch/scheduler.h>

volatile bool SchedulerRunning = false;
volatile thread_t *ActiveTasks[MAX_THREADS * 2];
static size_t TotalActiveTasks = 0;

volatile spinlock_t EntrySchedulerLock = INIT_SPINLOCK();
volatile spinlock_t SchedulerLock = INIT_SPINLOCK();
volatile spinlock_t SecondSchedulerLock = INIT_SPINLOCK();

static volatile thread_t *scheduler_get_next_task(kpid_t currentTask) {
    currentTask++;

    for(size_t i = 0; i < TotalActiveTasks; ) {
        volatile thread_t *thread = ActiveTasks[currentTask];
        if(thread == NULL) {
            currentTask = 0;
            thread = ActiveTasks[currentTask];
            if(thread == NULL) break; //There are no more tasks
        }
        if((void *)thread == (void *)-1) goto skip;
        if(spinlock_try_lock_skippreempt(&thread->lock)) goto next;
        //TODO: Yield target
        //TODO: Events
        return thread;
        next: i++;
        skip: if(++currentTask == MAX_THREADS * 2) currentTask = 0;
    }

    return NULL;
}

void scheduler_schedule(irq_regs_t *regs) {
    spinlock_lock(&EntrySchedulerLock);

    arch_disable_interrupts();
    arch_cpu_decrement_preempt_counter();

    if(!spinlock_try_lock_skippreempt(&SchedulerLock)) {
        spinlock_unlock_skippreempt(&EntrySchedulerLock);
        //TODO: Check if we can enable interrupts here
        return;
    }

    volatile kpid_t currentPid = arch_cpu_get_current_pid();
    volatile ktid_t currentTid = arch_cpu_get_current_tid();
    volatile ktid_t currentTaskID = arch_cpu_get_current_taskid();

    if(currentTaskID != -1) {
        volatile thread_t *thread = ActiveTasks[currentTaskID];
        if(thread == NULL || thread == (void *)-1) goto skip;

        thread->saved_regs = *regs;
        thread->cpu_number = -1;

        //TODO: SIMD
        if(currentPid) {
            thread->ustack_address = (void *)arch_cpu_get_thread_user_stack();
        }

        spinlock_unlock_skippreempt(&thread->lock);
    }
    skip: (void)0;

    volatile thread_t *nextTask = scheduler_get_next_task(currentTaskID);
    if(nextTask == NULL) {
        arch_scheduler_idle();
    }

    arch_cpu_set_current_pid(nextTask->pid);
    arch_cpu_set_current_tid(nextTask->tid);
    arch_cpu_set_current_taskid(nextTask->taskID);

    //TODO: SIMD
    if(nextTask->pid != 0) {
        arch_cpu_set_thread_kernel_stack((void *)nextTask->kstack_address);
        arch_cpu_set_thread_user_stack((void *)nextTask->ustack_address);

        //TODO: Fs base for x86_64
    }

    nextTask->cpu_number = arch_cpu_get_index();

    process_t *process = process_get_process(nextTask->pid);
    process_t *currentProcess = (currentPid != -1) ? process_get_process(currentPid) : NULL;
    //TODO: Check NULL
    spinlock_unlock(&process->lock);
    if(currentProcess) spinlock_unlock(&currentProcess->lock);

    if(currentPid == -1 || process->page_table != currentProcess->page_table) {
        arch_scheduler_switch_task(&nextTask->saved_regs, process->page_table->entries);
    } else {
        arch_scheduler_switch_task(&nextTask->saved_regs, NULL);
    }
}