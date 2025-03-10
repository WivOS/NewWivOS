CFLAGS += -mno-sse -mno-sse2 -mno-mmx -mno-80387 -m64
CFLAGS += -mno-red-zone -mcmodel=kernel

CFLAGS += -D__WIVOS__

LDFLAGS += -z max-page-size=0x1000
LDFLAGS += -Wl,configs/kernel_linker_x86_64.ld

include kernel/arch/x86_64/sources.mk