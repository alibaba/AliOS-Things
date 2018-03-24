/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _ESPCONN_BUF_H_
#define _ESPCONN_BUF_H_

#include "c_types.h"

typedef struct ringbuf_t {
    uint8_t* buf;
    uint8_t* head, *tail;
    size_t size;
} ringbuf, *ringbuf_t;

ringbuf_t ringbuf_new(size_t capacity);

size_t ringbuf_buffer_size(const struct ringbuf_t* rb);

void ringbuf_reset(ringbuf_t rb);

void ringbuf_free(ringbuf_t* rb);

size_t ringbuf_capacity(const struct ringbuf_t* rb);

size_t ringbuf_bytes_free(const struct ringbuf_t* rb);

size_t ringbuf_bytes_used(const struct ringbuf_t* rb);

int ringbuf_is_full(const struct ringbuf_t* rb);

int ringbuf_is_empty(const struct ringbuf_t* rb);

const void* ringbuf_tail(const struct ringbuf_t* rb);

const void* ringbuf_head(const struct ringbuf_t* rb);

static uint8_t* ringbuf_nextp(ringbuf_t rb, const uint8_t* p);

size_t ringbuf_findchr(const struct ringbuf_t* rb, int c, size_t offset);

size_t ringbuf_memset(ringbuf_t dst, int c, size_t len);

void* ringbuf_memcpy_into(ringbuf_t dst, const void* src, size_t count);

void* ringbuf_memcpy_from(void* dst, ringbuf_t src, size_t count);

#endif /* RINGBUF_H_ */
