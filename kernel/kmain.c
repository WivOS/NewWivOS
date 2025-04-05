#include <utils/system.h>

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

    printf("Waiting for rescheduling\n");

    while(1);
}

void kmain_thread_1() {
    printf("Thread 1\n");
    while(1);
}

void kmain_thread_2() {
    printf("Thread 2\n");
    while(1);
}

void kmain_thread_3() {
    printf("Thread 3\n");
    while(1);
}

void kmain_thread_4() {
    printf("Thread 4\n");
    while(1);
}

void kmain_threaded() {
    scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)kmain_thread_1, 0)));
    scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)kmain_thread_2, 0)));
    scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)kmain_thread_3, 0)));
    scheduler_add_task(0, thread_create(0, thread_parameter_entry, thread_entry_data((void *)kmain_thread_4, 0)));

    printf("Reached kernel end: Work in progress\n");

    while(1);
}