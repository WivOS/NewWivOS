#include <utils/dtb.h>

#include <utils/string.h>
#include <utils/system.h>
#include <utils/utils.h>
#include <mem/alloc.h>

gentree_t *DTBNodes = NULL;

static char *get_string_from_dtb(fdt_header_t *header, uint32_t nameoff) {
    return (char *)((uintptr_t)header + be32(header->off_dt_strings) + nameoff);
}

dtb_node_t *dtb_find_node(char *name, int index, int exact) {
    if(DTBNodes == NULL) return NULL;

    char *temp_name = strdup(name);
    char *freeFilename = temp_name;
    if(temp_name[0] == '/') temp_name++;

    spinlock_lock(&DTBNodes->lock);
    gentree_node_t *currentNode = (gentree_node_t *)DTBNodes->root;

    int currentIndex = 0;
    int first = 1;

    while(true) {
        char *node_to_match = strsep(&temp_name, "/");
        if(node_to_match == NULL || !strcmp(node_to_match, "")) break;

        int found = 0;
        foreach(list_value, currentNode->children) {
            gentree_node_t *gentree_node = (gentree_node_t *)list_value->value;
            dtb_node_t *node = (dtb_node_t *)gentree_node->value;
            if(!exact) {
                if(((strlen((char *)node->name) > strlen(node_to_match) &&
                   node->name[strlen(node_to_match)] == '@') || strlen((char *)node->name) <= strlen(node_to_match)) &&
                   !strncmp(node->name, node_to_match, strlen(node_to_match))) {
                    if(index == currentIndex) {
                        currentNode = gentree_node;
                        found = 1;
                        break;
                    } else {
                        currentIndex++;
                    }
                }
            }
            if(exact && strcmp((char *)node->name, node_to_match)) {
                currentNode = gentree_node;
                found = 1;
                break;
            }
        }

        if(!found) {
            //If we reach here there are no match
            kfree(freeFilename);
            spinlock_unlock(&DTBNodes->lock);

            return NULL;
        }
    }

    dtb_node_t *nodeResult = (dtb_node_t *)currentNode->value;

    spinlock_unlock(&DTBNodes->lock);

    kfree(freeFilename);
    return nodeResult;
}

int dtb_prop_read_u32(dtb_node_t *node, char *name, uint32_t *value, int offset) {
    if(DTBNodes == NULL) return -1;
    dtb_prop_t *prop = NULL;

    int found = 0;
    spinlock_lock(&node->props->lock);
    foreach(list_value, node->props) {
        prop = (dtb_prop_t *)list_value->value;
        if(!strcmp(name, prop->name)) {
            found = 1;
            break;
        }
    }
    spinlock_unlock(&node->props->lock);

    if(!found) return -1;
    if((offset + sizeof(uint32_t)) > prop->data_len) return -1;

    if(value) *value = be32(((uint32_t *)prop->data)[offset]);
    return 0;
}

int dtb_prop_read_u64(dtb_node_t *node, char *name, uint64_t *value, int offset) {
    if(DTBNodes == NULL) return -1;
    dtb_prop_t *prop = NULL;

    int found = 0;
    spinlock_lock(&node->props->lock);
    foreach(list_value, node->props) {
        prop = (dtb_prop_t *)list_value->value;
        if(!strcmp(name, prop->name)) {
            found = 1;
            break;
        }
    }
    spinlock_unlock(&node->props->lock);

    if(!found) return -1;
    if((offset + sizeof(uint32_t)) > prop->data_len) return -1;

    if(value) *value = be64(*((uint64_t *)&((uint32_t *)prop->data)[offset]));
    return 0;
}

int dtb_prop_read_address_range(dtb_node_t *node, char *name, uint32_t address_cells, uint32_t size_cells, uint64_t *address, uint64_t *size) {
    int address_offset = 0;
    int size_offset = address_cells;
    if(address != NULL)
        switch(address_cells) {
            case 0: *address = 0; break;
            case 1: {
                uint32_t value = 0;
                if(dtb_prop_read_u32(node, name, &value, address_offset) < 0) return -1;
                *address = value;
                break;
            }
            case 2: {
                if(dtb_prop_read_u64(node, name, address, address_offset) < 0) return -1;
                break;
            }
        }
    if(size != NULL)
        switch(size_cells) {
            case 0: *size = 0; break;
            case 1: {
                uint32_t value = 0;
                if(dtb_prop_read_u32(node, name, &value, size_offset) < 0) return -1;
                *size = value;
                break;
            }
            case 2: {
                if(dtb_prop_read_u64(node, name, size, size_offset) < 0) return -1;
                break;
            }
        }
    return 0;
}

char *dtb_prop_read_string(dtb_node_t *node, char *name) {
    if(DTBNodes == NULL) return NULL;
    dtb_prop_t *prop = NULL;

    int found = 0;
    spinlock_lock(&node->props->lock);
    foreach(list_value, node->props) {
        prop = (dtb_prop_t *)list_value->value;
        if(!strcmp(name, prop->name)) {
            found = 1;
            break;
        }
    }
    spinlock_unlock(&node->props->lock);

    if(!found) return NULL;

    return (char *)prop->data;
}

char *dtb_prop_read_stringlist(dtb_node_t *node, char *name, int index) {
    if(DTBNodes == NULL) return NULL;
    dtb_prop_t *prop = NULL;

    int found = 0;
    spinlock_lock(&node->props->lock);
    foreach(list_value, node->props) {
        prop = (dtb_prop_t *)list_value->value;
        if(!strcmp(name, prop->name)) {
            found = 1;
            break;
        }
    }
    spinlock_unlock(&node->props->lock);

    if(!found) return NULL;

    char *current_str = (char *)prop->data;
    int current_index = 0;
    while(((uintptr_t)current_str - (uintptr_t)prop->data) < prop->data_len) {
        if(current_index == index) return current_str;
        current_index++;
        current_str += strlen(current_str) + 1;
    }
    return NULL;
}

int dtb_init(uintptr_t address) {
    fdt_header_t *header = (fdt_header_t *)address;
    if(be32(header->magic) != 0xD00DFEED) return -1;
    if(be32(header->last_comp_version) > 17) return -1;

    DTBNodes = gentree_create();

    size_t parsedSize = 0;

    int first_node_found = 0;
    dtb_node_t *currentNode = NULL;

    fdt_block_t *current_block = (fdt_block_t *)(address + be32(header->off_dt_struct));
    while(parsedSize < header->size_dt_struct) {
        uint32_t token = be32(current_block->token);
        size_t blockSize = sizeof(uint32_t);
        switch(token) {
            case FDT_BEGIN_NODE:
                {
                    blockSize += strlen((char *)current_block->data) + 1;

                    dtb_node_t *node = (dtb_node_t *)kmalloc(sizeof(dtb_node_t));
                    node->props = list_create();
                    node->name = strdup((char *)current_block->data);
                    node->rootNode = first_node_found == 0 ? 1 : 0;
                    node->tree_node = gentree_insert(DTBNodes, (currentNode != NULL) ? currentNode->tree_node : NULL, (void *)node);
                    currentNode = node;

                    first_node_found = 1;

                    //printf("FDT_BEGIN_NODE(%s)\n", (char *)current_block->data);
                }
                break;
            case FDT_PROP:
                {
                    fdt_prop_t *fdt_prop = (fdt_prop_t *)current_block->data;
                    blockSize += sizeof(fdt_prop_t);
                    blockSize += be32(fdt_prop->len);

                    dtb_prop_t *prop = kmalloc(sizeof(dtb_prop_t));
                    prop->name = strdup(get_string_from_dtb(header, be32(fdt_prop->nameoff)));
                    prop->data_len = be32(fdt_prop->len);
                    prop->data = kmalloc(prop->data_len);
                    memcpy((void *)prop->data, (void *)fdt_prop->value, prop->data_len);

                    //printf("FDT_PROP(%s) | len: %d\n", get_string_from_dtb(header, be32(fdt_prop->nameoff)), be32(fdt_prop->len));
                    list_push_back(currentNode->props, prop);
                }
                break;
            case FDT_END_NODE:
                //printf("FDT_END_NODE(%s)\n", (char *)currentNode->name);
                if(currentNode->rootNode == 0)
                    currentNode = (dtb_node_t *)gentree_get_parent(DTBNodes, currentNode->tree_node, NULL)->value;
                break;
        }
        blockSize = ROUND_UP(blockSize, sizeof(uint32_t));

        if(token == FDT_END) break;

        parsedSize += blockSize;
        current_block = (fdt_block_t *)(address + be32(header->off_dt_struct) + parsedSize);
    }

    return 0;
}