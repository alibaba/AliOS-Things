/**
 * \file            lwesp_pbuf.h
 * \brief           Packet buffer manager
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
#ifndef LWESP_HDR_PBUF_H
#define LWESP_HDR_PBUF_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_PBUF Packet buffer
 * \brief           Packet buffer manager
 * \{
 */

lwesp_pbuf_p      lwesp_pbuf_new(size_t len);
size_t          lwesp_pbuf_free(lwesp_pbuf_p pbuf);
void*           lwesp_pbuf_data(const lwesp_pbuf_p pbuf);
size_t          lwesp_pbuf_length(const lwesp_pbuf_p pbuf, uint8_t tot);
uint8_t         lwesp_pbuf_set_length(lwesp_pbuf_p pbuf, size_t new_len);
lwespr_t          lwesp_pbuf_take(lwesp_pbuf_p pbuf, const void* data, size_t len, size_t offset);
size_t          lwesp_pbuf_copy(lwesp_pbuf_p pbuf, void* data, size_t len, size_t offset);

lwespr_t          lwesp_pbuf_cat(lwesp_pbuf_p head, const lwesp_pbuf_p tail);
lwespr_t          lwesp_pbuf_chain(lwesp_pbuf_p head, lwesp_pbuf_p tail);
lwesp_pbuf_p      lwesp_pbuf_unchain(lwesp_pbuf_p head);
lwespr_t          lwesp_pbuf_ref(lwesp_pbuf_p pbuf);

uint8_t         lwesp_pbuf_get_at(const lwesp_pbuf_p pbuf, size_t pos, uint8_t* el);
size_t          lwesp_pbuf_memcmp(const lwesp_pbuf_p pbuf, const void* data, size_t len, size_t offset);
size_t          lwesp_pbuf_strcmp(const lwesp_pbuf_p pbuf, const char* str, size_t offset);
size_t          lwesp_pbuf_memfind(const lwesp_pbuf_p pbuf, const void* data, size_t len, size_t off);
size_t          lwesp_pbuf_strfind(const lwesp_pbuf_p pbuf, const char* str, size_t off);

uint8_t         lwesp_pbuf_advance(lwesp_pbuf_p pbuf, int len);
lwesp_pbuf_p      lwesp_pbuf_skip(lwesp_pbuf_p pbuf, size_t offset, size_t* new_offset);

void*           lwesp_pbuf_get_linear_addr(const lwesp_pbuf_p pbuf, size_t offset, size_t* new_len);

void            lwesp_pbuf_set_ip(lwesp_pbuf_p pbuf, const lwesp_ip_t* ip, lwesp_port_t port);

void            lwesp_pbuf_dump(lwesp_pbuf_p p, uint8_t seq);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_PBUF_H */
