#include <utils/system.h>

#include <fs/vfs.h>

#include <mem/pmm.h>
#include <mem/vmm.h>
#include <tasking/scheduler.h>

void kmain_threaded();
void kmain() {
    printf("Hello world\n");

    limine_memmap_response_t *memmap = limine_get_memmap();
    pmm_init(memmap->entries, memmap->entry_count);
    limine_kernel_address_response_t *kaddr = limine_get_kernel_info();
    vmm_init((void*)kaddr->virtual_base, (void*)kaddr->physical_base, memmap->entries, memmap->entry_count);

    //We have memory yeah
    arch_post_vmm();

    scheduler_init((void *)kmain_threaded);

    printf("[Kernel] Waiting for rescheduling\n");

    while(1);
}

static volatile uint32_t threadCountTest = 0;
void kmain_thread_test(size_t thread_number) {
    printf("Thread %d\n", thread_number);

    locked_inc((uint32_t *)&threadCountTest);
    while(1);
}

void kmain_threaded() {
    printf("[Kernel] Threaded phase reached\n");

    {
        printf("[Kernel] Testing thread creation\n");
        #define THREAD_TEST_COUNT 4
        for(size_t i = 0; i < THREAD_TEST_COUNT; i++) {
            scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)kmain_thread_test, (void *)(i + 1))));
        }
        while(locked_read((uint32_t *)&threadCountTest) < THREAD_TEST_COUNT);
    }

    vfs_init();
    vfs_print_tree();

    printf("[Kernel] Reached kernel end: Work in progress\n");

    while(1);
}