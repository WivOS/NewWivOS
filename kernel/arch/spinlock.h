#pragma once

#include <stdatomic.h>
#include <stdbool.h>

#include <arch/common.h>

#define _DEBUG_

#define DEADLOCK_MAX_ITER 0x4000000

typedef struct {
    const char *file;
    const char *func;
    int line;
    uint64_t cpu;
} last_acquirer_t;

typedef struct spinlock_debug {
    uint32_t lock;
    last_acquirer_t last_acquirer;
} spinlock_debug_t;

typedef struct spinlock_normal {
    uint32_t lock;
} spinlock_normal_t;

#ifdef _DEBUG_

typedef spinlock_debug_t spinlock_t;

#else

typedef spinlock_normal_t spinlock_t;

#endif

uint32_t locked_read(uint32_t *var);
uint32_t locked_write(uint32_t *var, uint32_t value);
uint32_t locked_inc(uint32_t *var);
uint32_t locked_dec(uint32_t *var);

uint32_t spinlock_try_lock_debug(volatile spinlock_debug_t *spinlock, const char *file, const char *func, int line);
void spinlock_lock_debug(volatile spinlock_debug_t *spinlock, const char *file, const char *func, const char *lockname, int line);

uint32_t spinlock_try_lock_normal(volatile spinlock_normal_t *spinlock);
void spinlock_lock_normal(volatile spinlock_normal_t *spinlock);

void spinlock_unlock(volatile spinlock_t *spinlock);

#ifdef _DEBUG_

#define spinlock_try_lock(LOCK) spinlock_try_lock_debug(LOCK, __FILE__, __func__, __LINE__)
#define spinlock_lock(LOCK) spinlock_lock_debug(LOCK, __FILE__, __func__, #LOCK, __LINE__)

#define INIT_SPINLOCK() ((spinlock_t){0, (last_acquirer_t){"N/A", "N/A", 0, -1}})

#else

#define spinlock_try_lock(LOCK) spinlock_try_lock_normal(LOCK)
#define spinlock_lock(LOCK) spinlock_lock_normal(LOCK)

#define INIT_SPINLOCK() ((spinlock_t){0})

#endif

void arch_cpu_increment_preempt_counter();
void arch_cpu_decrement_preempt_counter();
#define spinlock_try_lock_skippreempt(LOCK) ({ \
    uint32_t ret = spinlock_try_lock(LOCK); \
    if(!ret) arch_cpu_decrement_preempt_counter(); \
    ret; \
})

#define spinlock_lock_skippreempt(LOCK) ({ \
    spinlock_lock(LOCK); \
    arch_cpu_decrement_preempt_counter(); \
})

#define spinlock_unlock_skippreempt(LOCK) ({ \
    arch_cpu_increment_preempt_counter(); \
    spinlock_unlock(LOCK); \
})