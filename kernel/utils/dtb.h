#pragma once

#include <arch/common.h>
#include <utils/utils.h>

typedef struct {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} fdt_header_t;

typedef struct {
    uint64_t address;
    uint64_t size;
} fdt_reserve_entry_t;

typedef struct {
    uint32_t token;
    uint32_t data[];
} fdt_block_t;

typedef struct {
    uint32_t len;
    uint32_t nameoff;
    uint8_t value[];
} fdt_prop_t;

typedef struct
{
    const char* name;
    list_t *props;
    gentree_node_t *tree_node;
    int rootNode;
} dtb_node_t;

typedef struct
{
    const char* name;
    const void* data;
    size_t data_len;
} dtb_prop_t;

#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP        0x00000003
#define FDT_NOP         0x00000004
#define FDT_END         0x00000009

int dtb_init(uintptr_t address);

dtb_node_t *dtb_find_node(char *name, int index, int exact);

int dtb_prop_read_u32(dtb_node_t *node, char *name, uint32_t *value, int offset);
int dtb_prop_read_u64(dtb_node_t *node, char *name, uint64_t *value, int offset);
int dtb_prop_read_address_range(dtb_node_t *node, char *name, uint32_t address_cells, uint32_t size_cells, uint64_t *address, uint64_t *size);
char *dtb_prop_read_string(dtb_node_t *node, char *name);
char *dtb_prop_read_stringlist(dtb_node_t *node, char *name, int index);