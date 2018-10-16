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

uint32_t swp_bytes_u32(uint32_t value)
{
    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
           (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}

static void utf8_to_str(uint8_t *data, uint8_t len, char *result)
{
    memcpy(result, data, len);
}

void utf8_to_ssid(uint8_t *data, uint8_t len, char *ssid)
{
    utf8_to_str(data, len, ssid);
}

void utf8_to_pw(uint8_t *data, uint8_t len, char *pw)
{
    utf8_to_str(data, len, pw);
}
