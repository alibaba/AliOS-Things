/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_UTILS_H
#define BZ_UTILS_H

#define SET_U16_LE(data, val) {                              \
    *(uint8_t *)(data) = (uint8_t)(val & 0xFF);              \
    *((uint8_t *)(data) + 1) = (uint8_t)((val >> 8) & 0xFF); \
}

#define SET_U32_LE(data, val) {                               \
    *(uint8_t *)(data) = (uint8_t)(val & 0xFF);               \
    *((uint8_t *)(data) + 1) = (uint8_t)((val >> 8) & 0xFF);  \
    *((uint8_t *)(data) + 2) = (uint8_t)((val >> 16) & 0xFF); \
    *((uint8_t *)(data) + 3) = (uint8_t)((val >> 24) & 0xFF); \
}

#define SET_U32_BE(data, val) {                               \
    *(uint8_t *)(data) =  (uint8_t)((val >> 24) & 0xFF);      \
    *((uint8_t *)(data) + 1) = (uint8_t)((val >> 16) & 0xFF); \
    *((uint8_t *)(data) + 2) = (uint8_t)((val >> 8) & 0xFF);  \
    *((uint8_t *)(data) + 3) = (uint8_t)(val & 0xFF);         \
}

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

uint8_t hex2ascii(uint8_t digit);
void hex2string(uint8_t *hex, uint32_t len, uint8_t *str);
void utf8_to_pw(uint8_t *data, uint8_t len, char *pw);

void get_random(uint8_t *random, uint8_t random_len);

#endif  // BZ_UTILS_H
