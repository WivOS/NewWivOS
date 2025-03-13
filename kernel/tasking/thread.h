#pragma once

#include <arch/common.h>
#include <arch/defines.h>
#include <stdbool.h>

#include <arch/spinlock.h>
#include <arch/int.h>

#include <tasking/process.h>

typedef size_t ktid_t;

typedef struct thread {
    volatile irq_regs_t saved_regs;
    volatile uint32_t guardValue;
    volatile ktid_t tid;
    volatile kpid_t pid;
    volatile ktid_t taskID;
    volatile void *kstack_address;
    volatile void *ustack_address;
    volatile size_t cpu_number;
    volatile spinlock_t lock;
    //TODO: Add a custom field for each arch
} __attribute__((packed)) thread_t;

typedef enum {
    thread_parameter_entry,
    //thread_parameter_exec
} thread_parameter_types_t;

typedef struct {
    void *entry;
    void *arg;
} thread_parameter_entry_data_t;

//typedef struct {
//    void *entry;
//    const char **argv;
//    const char **envp;
//    elf_value_t *value;
//} thread_parameter_exec_data_t;

#define thread_entry_data(entry_, arg_) &((thread_parameter_entry_data_t){.entry=entry_, .arg=arg_})

ktid_t thread_create(kpid_t pid, thread_parameter_types_t dataType, void *data);