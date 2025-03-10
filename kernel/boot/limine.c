#include <boot/limine.h>

#include <arch/common.h>

__attribute__((section(".limine_reqs")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((section(".limine_reqs_start_marker"), aligned(8), used))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((section(".limine_reqs")))
static volatile limine_stack_size_request_t limine_stack_size_request = {
    .header = {
        .id = LIMINE_STACK_SIZE_REQUEST,
        .revision = 0,
        .response = NULL
    },
    .stack_size = 0x10000
};

__attribute__((section(".limine_reqs")))
static volatile limine_memmap_request_t limine_memmap_request = {
    .header = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

__attribute__((section(".limine_reqs")))
static volatile limine_kernel_address_request_t limine_kernel_address_request = {
    .header = {
        .id = LIMINE_KERNEL_ADDRESS_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

__attribute__((section(".limine_reqs")))
static volatile limine_rsdp_request_t limine_rsdp_request = {
    .header = {
        .id = LIMINE_RSDP_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

__attribute__((section(".limine_reqs")))
static volatile limine_hhdm_request_t limine_hhdm_request = {
    .header = {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

__attribute__((section(".limine_reqs")))
static volatile limine_mp_request_t limine_mp_request = {
    .header = {
        .id = LIMINE_MP_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

__attribute__((section(".limine_reqs_end_marker"), aligned(8), used))
static volatile LIMINE_REQUESTS_END_MARKER;

limine_memmap_response_t *limine_get_memmap() {
    return (limine_memmap_response_t *)limine_memmap_request.header.response;
}

uintptr_t limine_get_hhdm() {
    return ((limine_hhdm_response_t *)limine_hhdm_request.header.response)->offset;
}

limine_kernel_address_response_t *limine_get_kernel_info() {
    return (limine_kernel_address_response_t *)limine_kernel_address_request.header.response;
}

limine_response_header_t *limine_get_mp() {
    return (limine_response_header_t *)limine_mp_request.header.response;
}

void kernel_entry() {
    arch_init();

    while(1);
}