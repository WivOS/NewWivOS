#pragma once

#define test_bit(bitmap, bitpos) ({ \
    int ret; \
    __asm__ __volatile__("bt %2, %1;" : "=@ccc"(ret), "+m"(*(bitmap)) : "r"(bitpos) : "memory"); \
    ret; \
})

#define set_bit(bitmap, bitpos) ({ \
    int ret; \
    __asm__ __volatile__("bts %2, %1;" : "=@ccc"(ret), "+m"(*(bitmap)) : "r"(bitpos) : "memory"); \
    ret; \
})

#define reset_bit(bitmap, bitpos) ({ \
    int ret; \
    __asm__ __volatile__("btr %2, %1;" : "=@ccc"(ret), "+m"(*(bitmap)) : "r"(bitpos) : "memory"); \
    ret; \
})

#define write_cr(reg, val) ({ \
    __asm__ __volatile__("movq %0, %%cr" reg ";" : : "r" (val)); \
})

#define read_cr(reg) ({ \
    size_t cr; \
    __asm__ __volatile__("movq %%cr" reg ", %0;": "=r" (cr)); \
    cr; \
})

#define rdmsr(msr) ({ \
    uint32_t edx, eax; \
    __asm__ __volatile__("rdmsr;" : "=a" (eax), "=d" (edx) : "c" (msr)); \
    ((uint64_t)edx << 32) | eax; \
})

#define wrmsr(msr, value) ({ \
    __asm__ __volatile__("wrmsr;" :: "a" ((uint32_t)value), "d" ((uint32_t)(value >> 32)), "c" (msr)); \
})

#define wrxcr(i, value) ({ \
    asm volatile ("xsetbv" :: "a" ((uint32_t)(value)), "d" ((uint32_t)((value) >> 32)), "c" (i)); \
})

#define cpuid(leaf, subleaf, eax, ebx, ecx, edx) ({ \
    uint32_t cpuid_max; \
    asm volatile ("cpuid" : "=a" (cpuid_max) : "a" ((leaf) & 0x80000000) : "rbx", "rcx", "rdx"); \
    if ((leaf) > cpuid_max) \
        asm volatile ("cpuid" : "=a" (*(eax)), "=b" (*(ebx)), "=c" (*(ecx)), "=d" (*(edx)) : "a" ((leaf)), "c" ((subleaf))); \
    ((leaf) <= cpuid_max); \
})

#define load_fs_base(base) wrmsr(0xc0000100, base)

#define PAGE_SIZE 0x1000

#define VMM_FLAG_MASK (0xFFF | (1ull << 63))
#define VMM_ADDR_MASK ~(VMM_FLAG_MASK)

#define VMM_PRESENT (1ULL<<0)
#define VMM_WRITE (1ULL<<1)
#define VMM_READ 0
#define VMM_EXECUTE 0
#define VMM_USER (1ULL<<2)
#define VMM_PAT0 (1ULL<<3)
#define VMM_PAT1 (1ULL<<4)
#define VMM_PAT2 (1ULL<<7)
#define VMM_DIRTY (1ULL<<5)
#define VMM_LARGE (1ULL<<7)
#define VMM_NX (1ULL<<63)