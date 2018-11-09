/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "crc.h"

#define CRC16_SEED   0xFFFF
#define POLY16       0x1021

#define CRC32_SEED   0xFFFFFFFF
#define POLY32       0x04C11DB7

/** The poly is 0x1021 (x^16 + x^12 + x^5 + 1) */
uint16_t utils_crc16(uint8_t *buf, uint32_t length)
{
    int i;
    unsigned short shift = CRC16_SEED, data = 0, val = 0;
    for (i = 0; i < length; i++) {
        if ((i % 8) == 0) {
            data = (*buf++) << 8;
        }
        val = shift ^ data;
        shift = shift << 1;
        data = data << 1;
        if (val & 0x8000) {
            shift = shift ^ POLY16;
        }
    }
    return shift;
}

/** The poly is 0x04C11DB7 (X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X+1) */
uint32_t utils_crc32(uint8_t *buf, uint32_t length)
{
    uint8_t i;
    uint32_t crc = CRC32_SEED;      // Initial value

    while (length--) {
        crc ^= (uint32_t) (*buf++) << 24;   // crc ^=(uint32_t)(*data)<<24; data++;
        for (i = 0; i < 8; ++i) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ POLY32;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
