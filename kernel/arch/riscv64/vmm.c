#include <mem/vmm.h>
#include <mem/pmm.h>

#include <mem/alloc.h>
#include <arch/riscv64/defines.h>

#include <utils/system.h>

typedef struct {
    size_t levels[5];
} pt_offset_t;

#define VIRT_KRNL_BASE 0xFFFFFFFF80000000UL

typedef struct {
    uint64_t entries[512];
} pt_entries_t;

static size_t max_level = 3;
void arch_vmm_preinit() {
    switch(MEM_PHYS_OFFSET) {
        case 0xFFFFFFC000000000: max_level = 3; break;
        case 0xFFFF800000000000: max_level = 4; break;
        case 0xFF00000000000000: max_level = 5; break;
    }
}

pt_t *vmm_setup_pt() {
    pt_t *pt = kmalloc(sizeof(pt_t *));
    pt->entries = (void*)pmm_alloc(1);
    pt->lock = INIT_SPINLOCK();

    vmm_map(pt, VmmKernelBase, VmmPhysicalKernelBase, ROUND_UP(VmmKernelSize, 0x1000) / 0x1000, VMM_EXECUTE | VMM_READ | VMM_WRITE);
    vmm_map_huge(pt, (void*)MEM_PHYS_OFFSET, NULL, 4096 * 3, VMM_EXECUTE | VMM_READ | VMM_WRITE);

    return pt;
}

#define make_satp(mode, ppn) (((size_t)(mode) << 60) | ((size_t)(ppn) >> 12))
void vmm_set_pt(pt_t *pt) {
    uint64_t satp = ((uint64_t)(5 + max_level) << 60) | ((uint64_t)pt->entries >> 12);
    csr_write("satp", satp);
}

//More levels defined to be used on Sv48 and Sv57, not used right now
static pt_offset_t get_addr_offsets(void *addr) {
    uint64_t addrValue = (uint64_t)addr;

    pt_offset_t offset = {
        .levels = {
            (addrValue & ((size_t)0x1FF << 12)) >> 12,
            (addrValue & ((size_t)0x1FF << 21)) >> 21,
            (addrValue & ((size_t)0x1FF << 30)) >> 30,
            (addrValue & ((size_t)0x1FF << 39)) >> 39,
            (addrValue & ((size_t)0x1FF << 48)) >> 48,
        }
    };

    return offset;
}

static pt_entries_t *vmm_get_or_alloc(pt_entries_t *pt, size_t offset, uint32_t flags) {
    uint64_t addr = pt->entries[offset];
    if(!(addr & VMM_PRESENT)) {
        addr = (uint64_t)pmm_alloc(1);
        if(!addr) while(1);
        pt->entries[offset] = addr = addr >> 2;
        pt->entries[offset] |= flags | VMM_PRESENT;
    }

    addr &= ~VMM_FLAG_MASK;
    addr <<= 2;
    if(addr & MEM_PHYS_OFFSET)
        addr |= MEM_PHYS_OFFSET;

    return (pt_entries_t *)(addr + MEM_PHYS_OFFSET);
}

static pt_entries_t *vmm_get_or_null(pt_entries_t *pt, size_t offset) {
    uint64_t addr = pt->entries[offset];
    if(!(addr & VMM_PRESENT)) {
        return NULL;
    }

    addr &= ~VMM_FLAG_MASK;
    addr <<= 2;
    if(addr & MEM_PHYS_OFFSET)
        addr |= MEM_PHYS_OFFSET;

    return (pt_entries_t *)(addr + MEM_PHYS_OFFSET);
}

static bool vmm_map_advanced(pt_t *ptaddr, void *addr, void *phys, size_t size, uint64_t permissions, bool huge) {
    pt_t *pttemp = ptaddr;
    spinlock_lock(&ptaddr->lock);
    while(size--) {
        pt_offset_t offset = get_addr_offsets(addr);

        pt_entries_t *pt = (pt_entries_t *)((uint64_t)ptaddr->entries + MEM_PHYS_OFFSET);
        if(!pt) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        for(size_t i = (max_level - 1); i > 1; i--) {
            pt = vmm_get_or_alloc(pt, offset.levels[i], VMM_PRESENT);
            if(!pt) {
                spinlock_unlock(&ptaddr->lock);
                return false;
            }
        }
        if(huge) {
            pt->entries[offset.levels[1]] = ((uint64_t)phys >> 2) | (permissions & VMM_FLAG_MASK) | VMM_PRESENT;
        } else {
            pt_entries_t *level1 = vmm_get_or_alloc(pt, offset.levels[1], VMM_PRESENT);
            if(!pt) {
                spinlock_unlock(&ptaddr->lock);
                return false;
            }
            level1->entries[offset.levels[0]] = ((uint64_t)phys >> 2) | (permissions & VMM_FLAG_MASK) | VMM_PRESENT;
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

        pt_entries_t *pt = (pt_entries_t *)((uint64_t)ptaddr->entries + MEM_PHYS_OFFSET);
        if(!pt) {
            spinlock_unlock(&ptaddr->lock);
            return false;
        }
        for(size_t i = (max_level - 1); i > 1; i--) {
            pt = vmm_get_or_null(pt, offset.levels[i]);
            if(!pt) {
                spinlock_unlock(&ptaddr->lock);
                return false;
            }
        }

        bool huge = (pt->entries[offset.levels[1]] & (VMM_READ | VMM_WRITE | VMM_EXECUTE)) != 0;
        if(huge) {
            if(size < 0x1FF) {
                spinlock_unlock(&ptaddr->lock);
                return false; //We cannot unmap a single page from a large page
            }

            pt->entries[offset.levels[1]] = (uint64_t)0;
            size -= 0x200; // size is on normal pages
        } else {
            pt_entries_t *level1  = vmm_get_or_null(pt, offset.levels[1]);
            level1->entries[offset.levels[0]] = (uint64_t)0;
        }

        addr = (void *)((uint64_t)addr + (huge ? 0x200000 : 0x1000));
    }

    spinlock_unlock(&ptaddr->lock);

    return true;
}