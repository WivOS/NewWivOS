#include <arch/riscv64/int.h>
#include <arch/riscv64/defines.h>

#include <utils/system.h>
#include <utils/dtb.h>
#include <arch/riscv64/sbi.h>

#define INSTRUCTION_ADDR_MIASLIGNED 0
#define INSTRUCTION_ACCESS_FAULT    1
#define ILLEGAL_INSTRUCTION         2
#define BREAKPOINT                  3
#define LOAD_ADDR_MISALIGNED        4
#define LOAD_ACCESS_FAULT           5
#define STORE_AMO_ADDR_MISALIGNED   6
#define STORE_AMO_ACCESS_FAULT      7
#define ENV_CALL_FROM_U_MODE        8
#define ENV_CALL_FROM_S_MODE        9
#define INSTRUCTION_PAGE_FAULT      12
#define LOAD_PAGE_FAULT             13
#define STORE_AMO_PAGE_FAULT        15

#define SOFTWARE_INTERRUPT  1
#define TIMER_INTERRUPT     5
#define EXTERNAL_INTERRUPT  9

#define SSATUS_SIE (1 << 1)
#define SIE_STIE (1 << 5)

void arch_enable_interrupts() {
    csr_write("sstatus", csr_read("sstatus") | SSATUS_SIE);
}

void arch_disable_interrupts() {
    csr_write("sstatus", csr_read("sstatus") & ~(uint64_t)SSATUS_SIE);
}

static uint64_t last_timer_event = 0;
void arch_interrupt_init() {
    arch_disable_interrupts();
    csr_write("sscratch", 0); //Remove thread context on the sscratch, we are in kernel

    csr_write("stvec", (uintptr_t)riscv_isr);

    printf("[Interrupts] Inited\n");
    arch_enable_interrupts();

    //This will cause a trap
    //*((volatile uint8_t *)0x1000000000) = 0;

    // Clear pending timer interrupts
    riscv_sbi_set_timer(-1);

    //Enable timer interrupts and set timer to trigger at 1000 cycles
    csr_write("sie", csr_read("sie") | SIE_STIE);
    last_timer_event = 10000;
    riscv_sbi_set_timer(last_timer_event);
}

extern void riscv_clear_soft_interrupt();
static uintptr_t riscv_handle_software_int(irq_regs_t *regs) {
    riscv_clear_soft_interrupt();

    printf("TODO: Software interrupts\n");

    return (uintptr_t)regs;
}

static uint64_t tick_count = 0;
static uintptr_t riscv_handle_timer_int(irq_regs_t *regs) {
    tick_count++;

    if((tick_count % 1000) == 0) {
        printf("Timer 1s elapsed\n");
    }

    last_timer_event += 10000;
    riscv_sbi_set_timer(last_timer_event); //Clear and set new event

    return (uintptr_t)regs;
}

static uintptr_t riscv_handle_external_int(irq_regs_t *regs) {
    printf("TODO: PLIC interrupt\n");

    return (uintptr_t)regs;
}

static uintptr_t riscv_handle_illegal_instruction(irq_regs_t *regs) {
    printf("Exception: Illegal instruction(0x%lX), TODO: Support fixing this\n", regs->tval);
    while(1);

    return (uintptr_t)regs;
}

static uintptr_t riscv_handle_page_fault(irq_regs_t *regs) {
    printf("Exception: PAGE Fault(addr: 0x%lX, cause: 0x%lX)\n", regs->tval, regs->cause);

    return (uintptr_t)regs;
}

uintptr_t riscv_handle_trap(irq_regs_t *regs) {
    if((int64_t)regs->cause < 0) {
        //Interrupt
        uint64_t int_no = regs->cause & ~(1UL << 63);
        switch(int_no) {
            case SOFTWARE_INTERRUPT:
                regs = (irq_regs_t *)riscv_handle_software_int(regs);
                break;
            case TIMER_INTERRUPT:
                regs = (irq_regs_t *)riscv_handle_timer_int(regs);
                break;
            case EXTERNAL_INTERRUPT:
                regs = (irq_regs_t *)riscv_handle_external_int(regs);
                break;

            default:
                printf("Unknown interrupt: 0x%lX\n", regs->cause);
                while(1);
                break;
        }
    } else {
        //Exception
        switch(regs->cause) {
            case ILLEGAL_INSTRUCTION:
                regs = (irq_regs_t *)riscv_handle_illegal_instruction(regs);
                break;
            case INSTRUCTION_PAGE_FAULT:
            case LOAD_PAGE_FAULT:
            case STORE_AMO_PAGE_FAULT:
                regs = (irq_regs_t *)riscv_handle_page_fault(regs);
                break;
            case ENV_CALL_FROM_U_MODE:
                printf("TODO: Syscall\n");
                break;
            case STORE_AMO_ACCESS_FAULT:
            case LOAD_ACCESS_FAULT:
            case INSTRUCTION_ACCESS_FAULT:
                regs = (irq_regs_t *)riscv_handle_page_fault(regs);
                break;

            default:
                printf("Unknown exception: 0x%lX\n", regs->cause);
                while(1);
                break;
        }
    }

    return (uintptr_t)regs;
}