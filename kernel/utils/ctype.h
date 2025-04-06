#pragma once

#include <arch/common.h>

inline int isdigit(int c) {
    return (uint8_t)(c - '0') <= 9;
}

inline int isalpha(int c)
{
    return ((unsigned) c | 32) - 'a' < 26;
}