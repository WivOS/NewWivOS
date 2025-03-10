#pragma once

#include <arch/common.h>
#include <boot/limine.h>

#include <arch/spinlock.h>

typedef struct {
    void *entries;
    spinlock_t lock;
} pt_t;

void arch_vmm_preinit();

pt_t *vmm_setup_pt();
void vmm_free_pt(pt_t *pt);

void vmm_set_pt(pt_t *pt);

bool vmm_map(pt_t *pt, void *addr, void *phys, size_t size, uint64_t permissions);
bool vmm_map_huge(pt_t *pt, void *addr, void *phys, size_t size, uint64_t permissions);
bool vmm_unmap(pt_t *pt, void *addr, size_t size);

//TODO

//void vmm_read(pt_t *ptaddr, void *addr, uint8_t *buffer, size_t size);
//void *vmm_get_phys(pt_t *ptaddr, void *addr);
//
//void vmm_map_if_not_mapped(pt_t *pt, void *addr, void *phys, size_t size, uint64_t permissions);
//
//pt_t *vmm_copy_pt(pt_t *pt);