#pragma once

#ifdef __riscv
    #include <arch/riscv64/defines.h>
#elif __x86_64
    #include <arch/x86_64/defines.h>
#endif