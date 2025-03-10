#pragma once

#include <arch/common.h>

void *kmalloc(size_t size);
void kfree(void *addr);
void *krealloc(void *addr, size_t size);
void *kcalloc(size_t num, size_t nsize);