#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))
#define ROUND_DOWN(N,S) ((N / S) * S)

typedef struct debug_printf_function {
    void (*write)(struct debug_printf_function *, char);
    void *data;
    size_t currentPos;
    size_t size;
} debug_printf_function_t;

// Init function for every architecture, needs to be called at start
void arch_init();

// Post Vmm function for every architecture, needs to be called after vmm init
void arch_post_vmm();

//Debug function that each architecture should implement to be able to print early debug info
void arch_debug_printf_function(debug_printf_function_t *func, char c);

//Main kernel entry
void kmain();