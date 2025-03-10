#include <arch/riscv64/arch.h>

#include <utils/dtb.h>
#include <utils/system.h>
#include <mem/pmm.h>

#define PLIC_IE_OFFSET 0x02000
#define PLIC_IE_CONTEXT_STRIDE 0x80

#define PLIC_THRESHOLD_OFFSET 0x200000
#define PLIC_THRESHOLD_CONTEXT_STRIDE 0x1000

#define PLIC_CLAIM_OFFSET 0x200004
#define PLIC_COMPLETE_OFFSET 0x200004
#define PLIC_COMPLETE_CONTEXT_STRIDE 0x1000

#define PLIC_PRIORITY_OFFSET 0x0
#define PLIC_PRIORITY_SOURCE_STRIDE 0x4

static uintptr_t plic_address = 0;
static uintptr_t plic_size = 0;
static uint32_t plic_ndev = 0;

static int plic_init_dtb() {
    dtb_node_t *soc_node = dtb_find_node("/soc", 0, 0);
    dtb_node_t *plic_node = dtb_find_node("/soc/plic", 0, 0);

    if(plic_node == NULL || soc_node == NULL) {
        return -1;
    }

    uint32_t address_cells, size_cells = 0;
    if(dtb_prop_read_u32(soc_node, "#address-cells", &address_cells, 0) < 0) return -1;
    if(dtb_prop_read_u32(soc_node, "#size-cells", &size_cells, 0) < 0) return -1;

    if(dtb_prop_read_address_range(plic_node, "reg", address_cells, size_cells, &plic_address, &plic_size) < 0) return -1;

    if(dtb_prop_read_u32(plic_node, "riscv,ndev", &plic_ndev, 0) < 0) return -1;

    return 0;
}

void plic_init() {
    if(plic_init_dtb() < 0) {
        printf("Failed to find PLIC: TODO\n");
        while(1);
    }

    printf("Found PLIC(address: %lX, size: %lX)\n", plic_address, plic_size);
    plic_address += MEM_PHYS_OFFSET;
}

static void plic_set_priority(uint32_t id, uint32_t priority) {
    const uintptr_t offset = PLIC_PRIORITY_OFFSET + id * PLIC_PRIORITY_SOURCE_STRIDE;
    *((volatile uint32_t *)(plic_address + offset)) = priority;
}

uint32_t plic_claim() {
    //TODO: Per cpu struct, doing zero for now as we ane not enabling smp
    uint16_t context_id = 0;

    uintptr_t offset = PLIC_CLAIM_OFFSET + (context_id * PLIC_COMPLETE_CONTEXT_STRIDE);
    return *((volatile uint32_t *)(plic_address + offset));
}

uint32_t arch_interrupt_max() { return plic_ndev - 1; }
uint32_t arch_interrupt_min() { return 0; }

void arch_interrupt_enable(uint32_t id) {
    plic_set_priority(id, 1);

    //TODO: Per cpu struct, doing zero for now as we ane not enabling smp
    uint16_t context_id = 0;

    uintptr_t offset = PLIC_IE_OFFSET + (context_id * PLIC_IE_CONTEXT_STRIDE) + ((id >> 5) << 2);
    uint32_t shift = id & 0x1F;
    uint32_t mask = 1 << shift;

    *((volatile uint32_t *)(plic_address + offset)) |= mask;
}

void arch_interrupt_disable(uint32_t id) {
    plic_set_priority(id, 1);

    //TODO: Per cpu struct, doing zero for now as we ane not enabling smp
    uint16_t context_id = 0;

    uintptr_t offset = PLIC_IE_OFFSET + (context_id * PLIC_IE_CONTEXT_STRIDE) + ((id >> 5) << 2);
    uint32_t shift = id & 0x1F;
    uint32_t mask = 1 << shift;

    *((volatile uint32_t *)(plic_address + offset)) &= ~mask;
}

void arch_interrupt_complete(uint32_t id) {
    //TODO: Per cpu struct, doing zero for now as we ane not enabling smp
    uint16_t context_id = 0;

    uintptr_t offset = PLIC_COMPLETE_OFFSET + (context_id * PLIC_COMPLETE_CONTEXT_STRIDE);
    *((volatile uint32_t *)(plic_address + offset)) = id;
}