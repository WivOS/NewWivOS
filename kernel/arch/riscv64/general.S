    .globl get_cpu_struct
    .type get_cpu_struct, @function
get_cpu_struct:
    mv a0, tp
    ret

    .globl set_cpu_struct
    .type set_cpu_struct, @function
set_cpu_struct:
    mv tp, a0
    ret

    .globl riscv_clear_soft_interrupt
    .type riscv_clear_soft_interrupt, @function
riscv_clear_soft_interrupt:
    li t0, 0x2
    csrc sip, t0
    ret

    .globl riscv_sbicall
    .type riscv_sbicall, @function
riscv_sbicall:
.option norelax
        mv      t0, a0
        mv      t1, a1
        mv      a0, a2
        mv      a1, a3
        mv      a2, a4
        mv      a3, a5
        mv      a4, a6
        mv      a5, a7
        mv      a7, t0
        mv      a6, t1
        ecall
        ret

.section .note.GNU-stack,"",%progbits