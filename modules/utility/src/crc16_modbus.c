#include "crc16.h"

const static uint16_t crc_modbus_table[256] = { 
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 
};

uint16_t crc_modbus(uint8_t const *buffer, int32_t len)
{
    int32_t i = 0, ch = 0;
    uint16_t crc = 0xFFFF;
    for (i = 0; i < len - 2; i++) {
        ch = buffer[i];
        crc = (uint16_t)(crc_modbus_table[(ch ^ crc) & 0x0F] ^ (crc >> 4));
        crc = (uint16_t)(crc_modbus_table[((ch >> 4) ^ crc) & 0x0F] ^ (crc >> 4));
    }

    crc = (uint16_t)((crc & 0xFF) << 8 | (crc >> 8));
    return crc;
}