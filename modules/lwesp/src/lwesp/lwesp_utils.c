/**
 * \file            lwesp_utils.c
 * \brief           Utilities
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include <stdint.h>
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_utils.h"

/**
 * \brief           Convert `unsigned 32-bit` number to string
 * \param[in]       num: Number to convert
 * \param[out]      out: Output variable to save string
 * \param[in]       is_hex: Set to `1` to output hex, 0 otherwise
 * \param[in]       width: Width of output string.
 *                      When number is shorter than width, leading `0` characters will apply.
 *                      This parameter is valid only when formatting hex numbers
 * \return          Pointer to output variable
 */
char*
lwesp_u32_to_gen_str(uint32_t num, char* out, uint8_t is_hex, uint8_t width) {
    char* tmp = out;
    uint8_t i, y;

    /* Convert number to string */
    i = 0;
    tmp[0] = '0';
    if (num == 0) {
        ++i;
    } else {
        if (is_hex) {
            uint8_t mod;
            while (num > 0) {
                mod = num & 0x0F;
                if (mod < 10) {
                    tmp[i] = mod + '0';
                } else {
                    tmp[i] = mod - 10 + 'A';
                }
                num >>= 4;
                ++i;
            }
        } else {
            while (num > 0) {
                tmp[i] = (num % 10) + '0';
                num /= 10;
                ++i;
            }
        }
    }
    if (is_hex) {
        while (i < width) {
            tmp[i] = '0';
            ++i;
        }
    }
    tmp[i] = 0;

    /* Rotate string */
    y = 0;
    while (y < ((i + 1) / 2)) {
        char t = out[i - y - 1];
        out[i - y - 1] = tmp[y];
        tmp[y] = t;
        ++y;
    }
    out[i] = 0;
    return out;
}

/**
 * \brief           Convert `signed 32-bit` number to string
 * \param[in]       num: Number to convert
 * \param[out]      out: Output variable to save string
 * \return          Pointer to output variable
 */
char*
lwesp_i32_to_gen_str(int32_t num, char* out) {
    if (num < 0) {
        *out = '-';
        ++out;
        return lwesp_u32_to_gen_str(LWESP_U32(-num), out, 0, 0) - 1;
    } else {
        return lwesp_u32_to_gen_str(LWESP_U32(num), out, 0, 0);
    }
}
