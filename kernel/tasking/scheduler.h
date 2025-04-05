#pragma once

#include <tasking/process.h>
#include <tasking/thread.h>

extern volatile spinlock_t EntrySchedulerLock;
extern volatile spinlock_t SchedulerLock;

void scheduler_init(void *first_task);

ktid_t scheduler_add_task(kpid_t pid, ktid_t tid);

void scheduler_schedule(irq_regs_t *regs);