CFLAGS += -target riscv64-unknown-none -march=rv64imac -mabi=lp64 -mno-relax

CFLAGS += -D__WIVOS__

LDFLAGS += -z max-page-size=0x1000
LDFLAGS += -Wl,-m,elf64lriscv -Wl,--no-relax -Wl,configs/kernel_linker_riscv64.ld

include kernel/arch/riscv64/sources.mk