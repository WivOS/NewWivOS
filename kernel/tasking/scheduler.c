#include <tasking/scheduler.h>
#include <tasking/process.h>
#include <tasking/thread.h>

#include <mem/alloc.h>
#include <arch/cpu.h>
#include <arch/scheduler.h>

#include <utils/system.h>

volatile bool SchedulerRunning = false;
volatile thread_t *ActiveTasks[MAX_THREADS * 2];
static size_t TotalActiveTasks = 0;

volatile spinlock_t EntrySchedulerLock = INIT_SPINLOCK();
volatile spinlock_t SchedulerLock = INIT_SPINLOCK();
volatile spinlock_t SecondSchedulerLock = INIT_SPINLOCK();

void scheduler_init(void *first_task) {
    //TODO: Save default context

    process_init_process_table();
    for(ktid_t i = 0; i < MAX_THREADS * 2; i++) ActiveTasks[i] = (thread_t *)NULL;

    //TODO: Set irq interrupt to point to sched here

    process_create(KernelPageMap);
    scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)first_task, 0)));

    SchedulerRunning = true;
}

ktid_t scheduler_add_task(kpid_t pid, ktid_t tid) {
    process_t *process = process_get_process(pid);
    if(process == NULL) return -1;

    volatile thread_t *thread = process->threads[tid];
    if(thread == NULL || thread->taskID != -1) {
        spinlock_unlock(&process->lock);
        return -1;
    }

    spinlock_lock(&EntrySchedulerLock);
    ktid_t taskID = 0;
    for(; taskID < MAX_THREADS*2; taskID++) {
        if(ActiveTasks[taskID] == NULL || ActiveTasks[taskID] == (void *)-1) {
            thread->taskID = taskID;
            ActiveTasks[taskID] = thread;
            TotalActiveTasks++;

            spinlock_unlock(&EntrySchedulerLock);
            spinlock_unlock(&process->lock);
            return taskID;
        }
    }

    printf("Max task number allocated reached\n");
    spinlock_unlock(&EntrySchedulerLock);
    spinlock_unlock(&process->lock);
    return -1;
}

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

int printf_scheduler(const char *format, ...);
void scheduler_schedule(irq_regs_t *regs) {
    int irq_status = arch_disable_interrupts();

    if(!SchedulerRunning || arch_cpu_get_preempt_counter() > 0) {
        //printf_scheduler("[%d] Skipping\n", arch_cpu_get_index());
        if(irq_status) arch_enable_interrupts();
        return;
    }

    spinlock_lock_skippreempt(&EntrySchedulerLock);

    if(spinlock_try_lock_skippreempt(&SchedulerLock)) {
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

    printf_scheduler("[%d] Switching pid: %d, tid: %d, taskId: %d\n", arch_cpu_get_index(), nextTask->pid, nextTask->tid, nextTask->taskID);

    //TODO: SIMD
    if(nextTask->pid != 0) {
        arch_cpu_set_thread_kernel_stack((void *)nextTask->kstack_address);
        arch_cpu_set_thread_user_stack((void *)nextTask->ustack_address);

        //TODO: Fs base for x86_64
    }

    nextTask->cpu_number = arch_cpu_get_index();

    //TODO: Check NULL
    process_t *process = process_get_process(nextTask->pid);
    spinlock_unlock(&process->lock);

    //TODO: Check NULL
    process_t *currentProcess = NULL;
    if(nextTask->pid == currentPid)
        currentProcess = process;
    else if(currentPid != -1) {
        currentProcess = process_get_process(currentPid);
        if(currentProcess) spinlock_unlock(&currentProcess->lock);
    }

    if(currentPid == -1 || process->page_table != currentProcess->page_table) {
        arch_scheduler_switch_task(&nextTask->saved_regs, process->page_table->entries);
    } else {
        arch_scheduler_switch_task(&nextTask->saved_regs, NULL);
    }
}