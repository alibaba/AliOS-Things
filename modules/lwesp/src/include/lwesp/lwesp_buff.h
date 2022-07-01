/**
 * \file            lwesp_buff.h
 * \brief           Ring buffer manager
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
#ifndef LWESP_HDR_BUFF_H
#define LWESP_HDR_BUFF_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_BUFF Ring buffer
 * \brief           Generic ring buffer
 * \{
 */

/* --- Buffer unique part starts --- */
/**
 * \brief           Buffer function/typedef prefix string
 *
 * It is used to change function names in zero time to easily re-use same library between applications.
 * Use `#define BUF_PREF(x)    my_prefix_ ## x` to change all function names to (for example) `my_prefix_buff_init`
 *
 * \note            Modification of this macro must be done in header and source file aswell
 */
#define BUF_PREF(x)                     lwesp_ ## x
/* --- Buffer unique part ends --- */

uint8_t     BUF_PREF(buff_init)(BUF_PREF(buff_t)* buff, size_t size);
void        BUF_PREF(buff_free)(BUF_PREF(buff_t)* buff);
void        BUF_PREF(buff_reset)(BUF_PREF(buff_t)* buff);

/* Read/Write functions */
size_t      BUF_PREF(buff_write)(BUF_PREF(buff_t)* buff, const void* data, size_t btw);
size_t      BUF_PREF(buff_read)(BUF_PREF(buff_t)* buff, void* data, size_t btr);
size_t      BUF_PREF(buff_peek)(BUF_PREF(buff_t)* buff, size_t skip_count, void* data, size_t btp);

/* Buffer size information */
size_t      BUF_PREF(buff_get_free)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_full)(BUF_PREF(buff_t)* buff);

/* Read data block management */
void*       BUF_PREF(buff_get_linear_block_read_address)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_linear_block_read_length)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_skip)(BUF_PREF(buff_t)* buff, size_t len);

/* Write data block management */
void*       BUF_PREF(buff_get_linear_block_write_address)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_linear_block_write_length)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_advance)(BUF_PREF(buff_t)* buff, size_t len);

#undef BUF_PREF                                 /* Prefix not needed anymore */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_BUFF_H */
