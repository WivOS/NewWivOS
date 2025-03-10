#include <utils/utils.h>

uint32_t be32(uint32_t input) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return input;
#else
    uint32_t temp = 0;
    temp |= (input & 0xFF) << 24;
    temp |= (input & 0xFF00) << 8;
    temp |= (input & 0xFF0000) >> 8;
    temp |= (input & 0xFF000000) >> 24;
    return temp;
#endif
}

uint64_t be64(uint64_t input) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return input;
#else
    uint64_t temp = 0;
    temp |= ((input >>  0) & 0xFF) << 56;
    temp |= ((input >>  8) & 0xFF) << 48;
    temp |= ((input >> 16) & 0xFF) << 40;
    temp |= ((input >> 24) & 0xFF) << 32;
    temp |= ((input >> 32) & 0xFF) << 24;
    temp |= ((input >> 40) & 0xFF) << 16;
    temp |= ((input >> 48) & 0xFF) << 8;
    temp |= ((input >> 56) & 0xFF) << 0;
    return temp;
#endif
}