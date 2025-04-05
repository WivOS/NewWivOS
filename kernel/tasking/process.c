#include <tasking/process.h>
#include <mem/alloc.h>

#include <utils/system.h>

process_t *ProcessTable[MAX_PROCESSES];

static volatile spinlock_t process_lock;

void process_init_process_table() {
    spinlock_lock(&process_lock);

    for(kpid_t i = 0; i < MAX_PROCESSES; i++) ProcessTable[i] = (process_t *)NULL;

    spinlock_unlock(&process_lock);
}

kpid_t process_create(pt_t *page_table) {
    spinlock_lock(&process_lock);

    kpid_t pid = 0;
    for(; pid < MAX_PROCESSES; pid++) {
        if(ProcessTable[pid] == NULL) {
            goto done;
        }
    }
    printf("Max process number allocated reached\n");
    spinlock_unlock(&process_lock);
    return -1;

done: (void)pid;

    process_t *newProcess = (process_t *)kmalloc(sizeof(process_t));
    newProcess->threads = (volatile thread_t **)kcalloc(sizeof(thread_t), MAX_THREADS);
    newProcess->page_table = page_table;
    newProcess->pid = pid;
    for(size_t i = 0; i < MAX_THREADS; i++) newProcess->threads[i] = NULL;

    newProcess->current_alloc_address = BASE_ALLOC_ADDRESS;
    newProcess->alloc_lock = INIT_SPINLOCK();

    //newProcess->file_handles = kcalloc(sizeof(vfs_node_t *), MAX_FILE_HANDLES);
    //for(size_t i = 0; i < MAX_FILE_HANDLES; i++) newProcess->file_handles[i] = (void *)-1;
    //newProcess->file_handles_lock = INIT_SPINLOCK();

    newProcess->lock = INIT_SPINLOCK();
    ProcessTable[pid] = newProcess;
    spinlock_unlock(&process_lock);

    return pid;
}

process_t *process_get_process(kpid_t pid) {
    if(pid >= MAX_PROCESSES) return NULL;

    spinlock_lock(&process_lock);

    process_t *process = ProcessTable[pid];
    spinlock_lock(&process->lock);

    spinlock_unlock(&process_lock);

    return process;
}