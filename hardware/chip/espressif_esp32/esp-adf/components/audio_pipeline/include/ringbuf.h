/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
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

#ifndef _RINGBUF_H__
#define _RINGBUF_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RB_OK           (ESP_OK)
#define RB_FAIL         (ESP_FAIL)
#define RB_DONE         (-2)
#define RB_ABORT        (-3)
#define RB_TIMEOUT      (-4)

typedef struct ringbuf *ringbuf_handle_t;

/**
 * @brief      Create ringbuffer with total size = block_size * n_blocks
 *
 * @param[in]  block_size   Size of each block
 * @param[in]  n_blocks     Number of blocks
 *
 * @return     ringbuf_handle_t
 */
ringbuf_handle_t rb_create(int block_size, int n_blocks);

/**
 * @brief      Cleanup and free all memory created by ringbuf_handle_t
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t rb_destroy(ringbuf_handle_t rb);

/**
 * @brief      Abort waiting until there is space for reading or writing of the ringbuffer
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t rb_abort(ringbuf_handle_t rb);

/**
 * @brief      Reset ringbuffer, clear all values as initial state
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t rb_reset(ringbuf_handle_t rb);

/**
 * @brief      Get total bytes available of Ringbuffer
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return     total bytes available
 */
int rb_bytes_available(ringbuf_handle_t rb);

/**
 * @brief      Get the number of bytes that have filled the ringbuffer
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return     The number of bytes that have filled the ringbuffer
 */
int rb_bytes_filled(ringbuf_handle_t rb);

/**
 * @brief      Get total size of Ringbuffer (in bytes)
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return     total size of Ringbuffer
 */
int rb_get_size(ringbuf_handle_t rb);

/**
 * @brief      Read from Ringbuffer to `buf` with len and wait `tick_to_wait` ticks until enough bytes to read
 *             if the ringbuffer bytes available is less than `len`.
 *             If `buf` argument provided is `NULL`, then ringbuffer do pseudo reads by simply advancing pointers.
 *
 * @param[in]  rb             The Ringbuffer handle
 * @param      buf            The buffer pointer to read out data
 * @param[in]  len            The length request
 * @param[in]  ticks_to_wait  The ticks to wait
 *
 * @return     Number of bytes read
 */
int rb_read(ringbuf_handle_t rb, char *buf, int len, TickType_t ticks_to_wait);

/**
 * @brief      Write to Ringbuffer from `buf` with `len` and wait `tick_to_wait` ticks until enough space to write
 *             if the ringbuffer space available is less than `len`
 *
 * @param[in]  rb             The Ringbuffer handle
 * @param      buf            The buffer
 * @param[in]  len            The length
 * @param[in]  ticks_to_wait  The ticks to wait
 *
 * @return     Number of bytes written
 */
int rb_write(ringbuf_handle_t rb, char *buf, int len, TickType_t ticks_to_wait);

/**
 * @brief      Set status of writing to ringbuffer is done
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t rb_done_write(ringbuf_handle_t rb);

/**
 * @brief      Unblock from rb_read
 *
 * @param[in]  rb    The Ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t rb_unblock_reader(ringbuf_handle_t rb);


#ifdef __cplusplus
}
#endif

#endif
