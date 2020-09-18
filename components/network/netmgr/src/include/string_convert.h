/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef STRING_CONVERT_H
#define STRING_CONVERT_H

#include "stdint.h"

#define UNICODE_TO_GBK     0x0
#define UNICODE_BIG_TO_GBK 0x1
#define UTF8_TO_GBK        0x2
#define GBK_TO_UNICODE     0x3
#define UNKNOWN_CONVERT    0xFF

/**
 * Convert char to special encode format (GBK, UNICODE, ...)
 * @param[in] src_char           character code to be converted
 * @param[in] type               convert option: UNICODE_TO_GBK, GBK_TO_UNICODE, ...
 * @return   Converted code, 0 means conversion error
 */
uint16_t char_convert(uint16_t src_char, uint32_t type);

/**
 * Convert string to special encode format (GBK, UNICODE, ...)
 * @param[out] dst_str           converted string
 * @param[in]  max_dst_len       max dst string Length(Byte)
 * @param[in]  src_str           string to be converted
 * @param[in]  src_len           src string's length (Byte)
 * @param[in]  type              convert option: UNICODE_TO_GBK, GBK_TO_UNICODE, ...
 * @return   Converted string length, 0 means conversion error
 */
uint32_t string_convert(uint8_t* dst_str, uint32_t max_dst_len,
                        uint8_t* src_str, uint32_t src_len, uint32_t type);

#endif/* STRING_CONVERT_H */

