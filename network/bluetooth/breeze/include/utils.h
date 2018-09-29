/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_UTILS_H
#define BREEZE_UTILS_H

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

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#endif
