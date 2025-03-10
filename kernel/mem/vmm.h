#pragma once

#include <arch/vmm.h>

extern pt_t *KernelPageMap;
extern void *VmmKernelBase;
extern void *VmmPhysicalKernelBase;
extern size_t VmmKernelSize;

void vmm_init(void *kernelBase, void *physKernelBase, limine_memmap_entry_t **entries, size_t count);
pt_t *vmm_setup_pt();