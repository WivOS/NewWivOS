#include <arch/x86_64/arch.h>
#include <arch/spinlock.h>

uint32_t locked_read(uint32_t *var) {
    uint32_t ret = 0;
    __asm__ __volatile__("lock xadd %0, %1;" : "+r"(ret) : "m"(*var) : "memory", "cc");
    return ret;
}

uint32_t locked_write(uint32_t *var, uint32_t value) {
    uint32_t ret = 0;
    __asm__ __volatile__("lock xchg %0, %1;" : "+r"(ret) : "m"(*var) : "memory"); \
    return ret;
}

uint32_t locked_inc(uint32_t *var) {
    uint32_t ret = 0;
    __asm__ __volatile__("lock incl %1;" : "=@ccnz"(ret) : "m"(*var) : "memory");
    return ret;
}

uint32_t locked_dec(uint32_t *var) {
    uint32_t ret = 0;
    __asm__ __volatile__("lock decl %1;" : "=@ccnz"(ret) : "m"(*var) : "memory");
    return ret;
}

static uint32_t block = 0;

static void qemu_debug_puts_urgent(const char *str) {
    locked_inc(&block);
    for(size_t i = 0; str[i]; i++)
        outb(0xE9, str[i]);
    locked_dec(&block);
}

#define __puts_uint(val) ({ \
    char buf[21] = {0}; \
    int i; \
    int val_copy = (uint64_t)(val); \
    if (!val_copy) { \
        buf[0] = '0'; \
        buf[1] = 0; \
        i = 0; \
    } else { \
        for (i = 19; val_copy; i--) { \
            buf[i] = (val_copy % 10) + '0'; \
            val_copy /= 10; \
        } \
        i++; \
    } \
    qemu_debug_puts_urgent(buf + i); \
})

__attribute__((unused)) static uint32_t deadlock_detect_lock = 0;

__attribute__((noinline)) __attribute__((unused)) static void deadlock_detect(const char *file,
                        const char *function,
                        int line,
                        const char *lockname,
                        volatile spinlock_debug_t *lock,
                        size_t iter) {
    while (locked_write(&deadlock_detect_lock, 1));
    qemu_debug_puts_urgent("\n---\npossible deadlock at: spinlock_lock(");
    qemu_debug_puts_urgent(lockname);
    qemu_debug_puts_urgent(");");
    qemu_debug_puts_urgent("\nfile: ");
    qemu_debug_puts_urgent(file);
    qemu_debug_puts_urgent("\nfunction: ");
    qemu_debug_puts_urgent(function);
    qemu_debug_puts_urgent("\nline: ");
    __puts_uint(line);
    qemu_debug_puts_urgent("\n---\nlast acquirer:");
    qemu_debug_puts_urgent("\nfile: ");
    qemu_debug_puts_urgent(lock->last_acquirer.file);
    qemu_debug_puts_urgent("\nfunction: ");
    qemu_debug_puts_urgent(lock->last_acquirer.func);
    qemu_debug_puts_urgent("\nline: ");
    __puts_uint(lock->last_acquirer.line);
    qemu_debug_puts_urgent("\n---\nassumed locked after it spun for ");
    __puts_uint(iter);
    qemu_debug_puts_urgent(" iterations\n---\n");
    locked_write(&deadlock_detect_lock, 0);
}

uint32_t spinlock_try_lock_debug(volatile spinlock_debug_t *spinlock, const char *file, const char *func, int line) {
    uint32_t ret;
    __asm__ __volatile__("lock btsl $0, %0;" : "+m"(spinlock->lock), "=@ccc"(ret) :: "memory");
    if(!ret) {
        spinlock->last_acquirer.file = file;
        spinlock->last_acquirer.func = func;
        spinlock->last_acquirer.line = line;
    }
    return ret;
}

void spinlock_lock_debug(volatile spinlock_debug_t *spinlock, const char *file, const char *func, const char *lockname, int line) {
retry:
    for(size_t i = 0; i < DEADLOCK_MAX_ITER; i++)
        if(!spinlock_try_lock_debug(spinlock, file, func, line))
            goto out;
    deadlock_detect(file, func, line, lockname, spinlock, DEADLOCK_MAX_ITER);
    goto retry;
out:
    return;
}

uint32_t spinlock_try_lock_normal(volatile spinlock_normal_t *spinlock) {
    uint32_t ret = 0;
    __asm__ __volatile__("lock btsl $0, %0;" : "+m"(spinlock->lock) :: "memory");
    return ret;
}

void spinlock_lock_normal(volatile spinlock_normal_t *spinlock) {
    __asm__ __volatile__(
        "1: "
        "lock btrl $0, %0;"
        "jc 2f;"
        "pause;"
        "jmp 1b;"
        "2: "
        : "+m"(spinlock->lock)
        :: "memory", "cc"
    );
}

void spinlock_unlock(volatile spinlock_t *spinlock) {
    __asm__ __volatile__("lock btrl $0, %0;" : "+m"(spinlock->lock) :: "memory", "cc");
}