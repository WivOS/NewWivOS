#include <mem/vmm.h>
#include <mem/pmm.h>
#include <arch/common.h>
#include <arch/defines.h>
#include <utils/system.h>
#include <utils/string.h>

pt_t *KernelPageMap = NULL;
void *VmmKernelBase;
void *VmmPhysicalKernelBase;
size_t VmmKernelSize;

void vmm_init(void *kernelBase, void *physKernelBase, limine_memmap_entry_t **entries, size_t count) {
    arch_vmm_preinit();

    VmmKernelBase = kernelBase;
    VmmPhysicalKernelBase = physKernelBase;

    for(int i = 0; i < count; i++) {
        if(entries[i]->type == LIMINE_MEMMAP_KERNEL_AND_MODULES && entries[i]->base == (uint64_t)VmmPhysicalKernelBase) {
            if((uint64_t)VmmPhysicalKernelBase % 0x200000) {
                void *newPhysBase = pmm_alloc_advanced(ROUND_UP(entries[i]->length, PAGE_SIZE) / PAGE_SIZE, 0x200);
                VmmPhysicalKernelBase = newPhysBase;
            }
            VmmKernelSize = entries[i]->length;
            break;
        }
    }

    KernelPageMap = vmm_setup_pt();
    vmm_map_huge(KernelPageMap, NULL, NULL, 0x100, VMM_EXECUTE | VMM_READ | VMM_WRITE);
    memcpy((void*)((uint64_t)VmmPhysicalKernelBase + MEM_PHYS_OFFSET), VmmKernelBase, VmmKernelSize);

    vmm_set_pt(KernelPageMap);

    if(VmmPhysicalKernelBase != physKernelBase) pmm_free(physKernelBase, ROUND_UP(VmmKernelSize, PAGE_SIZE) / PAGE_SIZE);

    printf("[VMM] Inited\n");
}