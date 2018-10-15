/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "utils.h"

uint8_t hex2ascii(uint8_t digit)
{
    uint8_t val;

    if (digit <= 9) {
        val = digit - 0x0 + '0';
    } else {
        val = digit - 0xA + 'A';
    }

    return val;
}

void hex2string(uint8_t *hex, uint32_t len, uint8_t *str)
{
    uint32_t index;

    for (index = 0; index < len; index++) {
        str[index * 2] = hex2ascii(hex[index] >> 4 & 0x0f);
        str[index * 2 + 1] = hex2ascii(hex[index] & 0x0f);
    }
}
