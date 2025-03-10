#include <mem/vmm.h>
#include <mem/pmm.h>

#include <mem/alloc.h>
#include <arch/x86_64/defines.h>

typedef struct {
    size_t pml4_off;
    size_t pdp_off;
	size_t pd_off;
	size_t pt_off;
} pt_offset_t;

#define VIRT_KRNL_BASE 0xFFFFFFFF80000000UL

typedef struct {
    uint64_t entries[512];
} pt_entries_t;

void arch_vmm_preinit() {}

pt_t *vmm_setup_pt() {
    pt_t *pt = kmalloc(sizeof(pt_t *));
    pt->entries = (void*)pmm_alloc(1);
    pt->lock = INIT_SPINLOCK();

    vmm_map(pt, VmmKernelBase, VmmPhysicalKernelBase, ROUND_UP(VmmKernelSize, 0x1000) / 0x1000, 3);
    vmm_map_huge(pt, (void*)MEM_PHYS_OFFSET, NULL, 4096 * 3, 3);

    return pt;
}

void vmm_set_pt(pt_t *pt) {
    asm volatile ("mov %%rax, %%cr3" : : "a"(pt->entries) : "memory");
}

static pt_offset_t get_addr_offsets(void *addr) {
    uint64_t addrValue = (uint64_t)addr;

    pt_offset_t offset = {
        .pml4_off   = (addrValue & ((size_t)0x1FF << 39)) >> 39,
        .pdp_off    = (addrValue & ((size_t)0x1FF << 30)) >> 30,
        .pd_off     = (addrValue & ((size_t)0x1FF << 21)) >> 21,
        .pt_off     = (addrValue & ((size_t)0x1FF << 12)) >> 12
    };

    return offset;
}

static pt_entries_t *vmm_get_or_alloc(pt_entries_t *pt, size_t offset, uint32_t flags) {
    uint64_t addr = pt->entries[offset];
    if(!(addr & VMM_PRESENT)) {
        addr = pt->entries[offset] = (uint64_t)pmm_alloc(1);
        if(!addr) while(1);
        pt->entries[offset] |= flags | VMM_PRESENT;
    }

    return (pt_entries_t *)((addr & VMM_ADDR_MASK) + MEM_PHYS_OFFSET);
}

static pt_entries_t *vmm_get_or_null(pt_entries_t *pt, size_t offset) {
    uint64_t addr = pt->entries[offset];
    if(!(addr & VMM_PRESENT)) {
        return NULL;
    }

    return (pt_entries_t *)((addr & VMM_ADDR_MASK) + MEM_PHYS_OFFSET);
}

static bool vmm_map_advanced(pt_t *ptaddr, void *addr, void *phys, size_t size, uint64_t permissions, bool huge) {
    spinlock_lock(&ptaddr->lock);
    while(size--) {
        pt_offset_t offset = get_addr_offsets(addr);

        pt_entries_t *pml4 = (pt_entries_t *)((uint64_t)ptaddr->entries + MEM_PHYS_OFFSET);
        if(!pml4) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        pt_entries_t *pdp = vmm_get_or_alloc(pml4, offset.pml4_off, VMM_WRITE | VMM_USER);
        if(!pdp) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        pt_entries_t *pd  = vmm_get_or_alloc(pdp, offset.pdp_off, VMM_WRITE | VMM_USER);
        if(!pd) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        if(huge) {
            pd->entries[offset.pd_off] = (uint64_t)phys | permissions | VMM_PRESENT | VMM_LARGE;
        } else {
            pt_entries_t *pt  = vmm_get_or_alloc(pd, offset.pd_off, VMM_WRITE | VMM_USER);
            if(!pt) {
                spinlock_unlock(&ptaddr->lock);
                return false;
            }
            pt->entries[offset.pt_off] = (uint64_t)phys | permissions | VMM_PRESENT;
        }

        addr = (void *)((uint64_t)addr + (huge ? 0x200000 : 0x1000));
        phys = (void *)((uint64_t)phys + (huge ? 0x200000 : 0x1000));
    }
    spinlock_unlock(&ptaddr->lock);

    return true;
}

bool vmm_map(pt_t *pt, void *addr, void *phys, size_t size, uint64_t permissions) {
    return vmm_map_advanced(pt, addr, phys, size, permissions, false);
}

bool vmm_map_huge(pt_t *pt, void *addr, void *phys, size_t size, uint64_t permissions) {
    return vmm_map_advanced(pt, addr, phys, size, permissions, true);
}

bool vmm_unmap(pt_t *ptaddr, void *addr, size_t size) {
    spinlock_lock(&ptaddr->lock);
    while(size-- > 0) {
        pt_offset_t offset = get_addr_offsets(addr);

        pt_entries_t *pml4 = (pt_entries_t *)((uint64_t)ptaddr->entries + MEM_PHYS_OFFSET);
        if(!pml4) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        pt_entries_t *pdp = vmm_get_or_null(pml4, offset.pml4_off);
        if(!pdp) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        pt_entries_t *pd  = vmm_get_or_null(pdp, offset.pdp_off);
        if(!pd) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }

        bool huge = (pd->entries[offset.pd_off] & VMM_LARGE);
        if(huge) {
            if(size < 0x1FF) {
                spinlock_unlock(&ptaddr->lock);
                return false; //We cannot unmap a singl page from a large page
            }

            pd->entries[offset.pd_off] = (uint64_t)0;
            size -= 0x200; // size is on normal pages
        } else {
            pt_entries_t *pt  = vmm_get_or_null(pd, offset.pd_off);
            pt->entries[offset.pt_off] = (uint64_t)0;
        }

        addr = (void *)((uint64_t)addr + (huge ? 0x200000 : 0x1000));
    }

    spinlock_unlock(&ptaddr->lock);

    return true;
}