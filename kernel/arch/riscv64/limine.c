#include <boot/limine.h>
#include <utils/system.h>

__attribute__((section(".limine_reqs")))
static volatile limine_dtb_request_t limine_dtb_request = {
    .header = {
        .id = LIMINE_DTB_REQUEST,
        .revision = 0,
        .response = NULL
    }
};

void *riscv64_get_boot_dtb() {
    if(limine_dtb_request.header.response != NULL)
        return ((limine_dtb_response_t *)limine_dtb_request.header.response)->dtb_ptr;

    printf("Error: DTB not passed\n");

    return NULL;
}