#pragma once

#include <arch/common.h>
#include <tasking/process.h>
#include <tasking/thread.h>

void arch_cpu_increment_preempt_counter();
void arch_cpu_decrement_preempt_counter();
size_t arch_cpu_get_preempt_counter();

size_t arch_cpu_get_index();

kpid_t arch_cpu_get_current_pid();
ktid_t arch_cpu_get_current_tid();
ktid_t arch_cpu_get_current_taskid();
void *arch_cpu_get_thread_user_stack();
void *arch_cpu_get_thread_kernel_stack();

void arch_cpu_set_current_pid(kpid_t kpid);
void arch_cpu_set_current_tid(ktid_t ktid);
void arch_cpu_set_current_taskid(ktid_t taskID);
void arch_cpu_set_thread_user_stack(void *address);
void arch_cpu_set_thread_kernel_stack(void *address);