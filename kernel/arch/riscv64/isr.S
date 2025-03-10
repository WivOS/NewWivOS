.macro RISCV_SAVE_CONTEXT user
.if \user == 1
    sd sp, 8(tp)
    ld sp, 16(tp)
.else
    csrrw tp, sscratch, tp
.endif

    addi sp, sp, -280
    sd x1, 0(sp)

.if \user == 1
    ld x1, 8(tp)
    sd x1, 8(sp)
    csrrw tp, sscratch, tp
    sd tp, 24(sp)
    csrrw tp, sscratch, tp
.else
    addi x1, sp, 280
    sd x1, 8(sp)
    sd tp, 24(sp)
.endif

    sd x3, 16(sp)
    sd x5, 32(sp)
    sd x6, 40(sp)
    sd x7, 48(sp)
    sd x8, 56(sp)
    sd x9, 64(sp)
    sd x10, 72(sp)
    sd x11, 80(sp)
    sd x12, 88(sp)
    sd x13, 96(sp)
    sd x14, 104(sp)
    sd x15, 112(sp)
    sd x16, 120(sp)
    sd x17, 128(sp)
    sd x18, 136(sp)
    sd x19, 144(sp)
    sd x20, 152(sp)
    sd x21, 160(sp)
    sd x22, 168(sp)
    sd x23, 176(sp)
    sd x24, 184(sp)
    sd x25, 192(sp)
    sd x26, 200(sp)
    sd x27, 208(sp)
    sd x28, 216(sp)
    sd x29, 224(sp)
    sd x30, 232(sp)
    sd x31, 240(sp)

    # Zero-out scratch when entering the kernel
    csrw sscratch, zero

    csrr a0, sepc
    sd a0, 248(sp)
    csrr a0, scause
    sd a0, 256(sp)
    csrr a0, stval
    sd a0, 264(sp)
    csrr a0, sstatus
    sd a0, 272(sp)
.endm

.macro RISCV_RESTORE_CONTEXT
    ld x1, 0(sp)

    ld x3, 16(sp)
    ld x5, 32(sp)
    ld x6, 40(sp)
    ld x7, 48(sp)
    ld x8, 56(sp)
    ld x9, 64(sp)
    ld x10, 72(sp)
    ld x11, 80(sp)
    ld x12, 88(sp)
    ld x13, 96(sp)
    ld x14, 104(sp)
    ld x15, 112(sp)
    ld x16, 120(sp)
    ld x17, 128(sp)
    ld x18, 136(sp)
    ld x19, 144(sp)
    ld x20, 152(sp)
    ld x21, 160(sp)
    ld x22, 168(sp)
    ld x23, 176(sp)
    ld x24, 184(sp)
    ld x25, 192(sp)
    ld x26, 200(sp)
    ld x27, 208(sp)
    ld x28, 216(sp)
    ld x29, 224(sp)
    ld x30, 232(sp)

    # Disable interrupts
    csrc sstatus, 1 << 1

    ld x31, 272(sp)
    andi x31, x31, 1 << 8
    bnez x31, 5f

    # Fill sscratch if we're returning to user space
    csrw sscratch, tp

5:
    ld x31, 240(sp)
    ld tp, 248(sp)
    csrw sepc, tp
    ld tp, 256(sp)
    csrw scause, tp
    ld tp, 264(sp)
    csrw stval, tp
    ld tp, 272(sp)
    csrw sstatus, tp
    ld tp, 24(sp)
    ld sp, 8(sp)
.endm

    .text
    .balign 4
    .type riscv_isr, @function
    .globl riscv_isr
riscv_isr:
    # sscratch is zero on kernel and contains thread pointer in user mode
    csrrw tp, sscratch, tp

    bnez tp, 1f
    # Here we are on kernel mode
    RISCV_SAVE_CONTEXT 0
    j 2f
1:
    # Here we are on user mode
    RISCV_SAVE_CONTEXT 1
2:

.option push
.option norelax
    la gp, __global_pointer$
.option pop
    mv a0, sp
    andi sp, sp, -16
    call riscv_handle_trap

    # this function recturn the old stack or the new stack

    mv sp, a0

    RISCV_RESTORE_CONTEXT
    sret
