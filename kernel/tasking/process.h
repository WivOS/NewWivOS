#pragma once

#include <arch/common.h>
#include <arch/spinlock.h>
#include <stdbool.h>

#include <mem/vmm.h>
#include <tasking/thread.h>

#define BASE_ALLOC_ADDRESS ((size_t)0x0000780000000000)

#define MAX_FILE_HANDLES 256

#ifndef kpid_t
typedef size_t kpid_t;
#endif
#ifndef ktid_t
typedef size_t ktid_t;
#endif

typedef struct process {
    volatile kpid_t pid;
    volatile pt_t *page_table;
    volatile thread_t **threads;
    volatile spinlock_t lock;
    volatile uint64_t current_alloc_address;
    volatile spinlock_t alloc_lock;
    //volatile vfs_node_t **file_handles;
    volatile kpid_t parent_pid;
} __attribute__((packed)) process_t;

#define MAX_PROCESSES 256
#define MAX_THREADS 256

extern process_t *ProcessTable[MAX_PROCESSES];

void process_init_process_table();

kpid_t process_create(pt_t *page_table);
process_t *process_get_process(kpid_t pid);
process_t *process_get_process_no_lock(kpid_t pid);