#pragma once

//TODO

#define test_bit(bitmap, bitpos) ((((volatile uint32_t *)bitmap)[bitpos >> 5] & ((uint32_t)(1 << ((bitpos) & 0x1F)))) != 0)

#define set_bit(bitmap, bitpos) (((volatile uint32_t *)bitmap)[bitpos >> 5] |= ((uint32_t)(1 << ((bitpos) & 0x1F))))

#define reset_bit(bitmap, bitpos) (((volatile uint32_t *)bitmap)[bitpos >> 5] &= ~((uint32_t)(1 << ((bitpos) & 0x1F))))

#define csr_read(csr) ({\
    size_t v;\
    asm volatile ("csrr %0, " csr : "=r"(v));\
    v;\
})

#define csr_write(csr, v) ({\
    size_t old;\
    asm volatile ("csrrw %0, " csr ", %1" : "=r"(old) : "r"(v));\
    old;\
})

#define PAGE_SIZE 0x1000

#define VMM_FLAG_MASK (0x3FF)

#define VMM_PRESENT (1ULL<<0)
#define VMM_READ (1ULL<<1)
#define VMM_WRITE (1ULL<<2)
#define VMM_EXECUTE (1ULL<<3)
#define VMM_USER (1ULL<<4)
#define VMM_FLOBAL (1ULL<<5)
#define VMM_ACCESSED (1ULL<<6)
#define VMM_DIRTY (1ULL<<7)

#include <arch/riscv64/int.h>