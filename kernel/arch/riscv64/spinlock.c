#include <arch/riscv64/arch.h>
#include <arch/spinlock.h>
#include <arch/int.h>
#include <arch/riscv64/cpu.h>

uint32_t locked_read(uint32_t *var) {
    return (uint32_t)__sync_fetch_and_or(var, 0);
}

uint32_t locked_write(uint32_t *var, uint32_t value) {
    return (uint32_t)__sync_val_compare_and_swap(var, *var, value);
}

uint32_t locked_inc(uint32_t *var) {
    return (uint32_t)__sync_add_and_fetch(var, 1);
}

uint32_t locked_dec(uint32_t *var) {
    return (uint32_t)__sync_sub_and_fetch(var, 1);
}

static uint32_t block = 0;

static unsigned char * debug_uart = (unsigned char *)0x10000000;
static void qemu_debug_puts_urgent(const char *str) {
    locked_inc(&block);
    for(size_t i = 0; str[i]; i++) {
        while(!(*((volatile uint8_t *)&debug_uart[5]) & (1 << 5)));
        *debug_uart = str[i];
    }
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
    int irq_status = arch_disable_interrupts();

    uint32_t ret;
    ret = __sync_lock_test_and_set(&spinlock->lock, 1);
    if(!ret) {
        spinlock->last_acquirer.file = file;
        spinlock->last_acquirer.func = func;
        spinlock->last_acquirer.line = line;

        //Disable preemption on this cpu
        if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter++;
    }
    if(irq_status) arch_enable_interrupts();
    return ret;
}

void spinlock_lock_debug(volatile spinlock_debug_t *spinlock, const char *file, const char *func, const char *lockname, int line) {
    int irq_status = arch_disable_interrupts();

    //Disable preemption on this cpu
    if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter++;

    if(irq_status) arch_enable_interrupts();

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
    int irq_status = arch_disable_interrupts();

    uint32_t value = (uint32_t)__sync_lock_test_and_set(&spinlock->lock, 1);

    if(!value && riscv_cpu_inited()) {
        //Disable preemption on this cpu
        get_cpu_struct()->preemptCounter++;
    }
    if(irq_status) arch_enable_interrupts();
    return value;
}

void spinlock_lock_normal(volatile spinlock_normal_t *spinlock) {
    int irq_status = arch_disable_interrupts();

    //Disable preemption on this cpu
    if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter++;

    if(irq_status) arch_enable_interrupts();

    while(__sync_lock_test_and_set(&spinlock->lock, 1) != 0);
}

void spinlock_unlock(volatile spinlock_t *spinlock) {
    __sync_lock_release(&spinlock->lock);

    int irq_status = arch_disable_interrupts();

    //Enable preemption on this cpu
    if(riscv_cpu_inited()) get_cpu_struct()->preemptCounter--;

    if(irq_status) arch_enable_interrupts();
}