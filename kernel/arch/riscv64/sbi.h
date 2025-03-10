#pragma once

#include <arch/common.h>

struct sbiret {
    int64_t error;
    int64_t value;
};

#define SBI_SUCCESS                 ((long)0)
#define SBI_ERR_FAILED              ((long)-1)
#define SBI_ERR_NOT_SUPPORTED       ((long)-2)
#define SBI_ERR_INVALID_PARAM       ((long)-3)
#define SBI_ERR_DENIED              ((long)-4)
#define SBI_ERR_INVALID_ADDRESS     ((long)-5)
#define SBI_ERR_ALREADY_AVAILABLE   ((long)-6)
#define SBI_ERR_ALREADY_STARTED     ((long)-7)
#define SBI_ERR_ALREADY_STOPPED     ((long)-8)

extern struct sbiret riscv_sbicall(int eid, int fid, ...);

#define SBI_EID_TIMER 0x54494D45

static inline struct sbiret riscv_sbi_set_timer(uint64_t stime_value) {
    return riscv_sbicall(SBI_EID_TIMER, 0, stime_value);
}