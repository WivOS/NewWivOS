#include <utils/system.h>

#include <mem/pmm.h>
#include <mem/vmm.h>

void kmain() {
    printf("Hello world\n");

    limine_memmap_response_t *memmap = limine_get_memmap();
    pmm_init(memmap->entries, memmap->entry_count);
    limine_kernel_address_response_t *kaddr = limine_get_kernel_info();
    vmm_init((void*)kaddr->virtual_base, (void*)kaddr->physical_base, memmap->entries, memmap->entry_count);

    //We have memory yeah
    arch_post_vmm();

    printf("Reached kernel end: Work in progress\n");

    while(1);
}