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

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "ringbuf.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "audio_error.h"

static const char *TAG = "RINGBUF";

struct ringbuf {
    char *p_o;                   /**< Original pointer */
    char *volatile p_r;          /**< Read pointer */
    char *volatile p_w;          /**< Write pointer */
    volatile uint32_t fill_cnt;  /**< Number of filled slots */
    uint32_t size;               /**< Buffer size */
    SemaphoreHandle_t can_read;
    SemaphoreHandle_t can_write;
    SemaphoreHandle_t lock;
    bool abort_read;
    bool abort_write;
    bool is_done_write;         /**< To signal that we are done writing */
    bool unblock_reader_flag;   /**< To unblock instantly from rb_read */
};

static esp_err_t rb_abort_read(ringbuf_handle_t rb);
static esp_err_t rb_abort_write(ringbuf_handle_t rb);
static void rb_release(SemaphoreHandle_t handle);

ringbuf_handle_t rb_create(int block_size, int n_blocks)
{
    if (block_size < 2) {
        ESP_LOGE(TAG, "Invalid size");
        return NULL;
    }

    ringbuf_handle_t rb;
    char *buf = NULL;
    bool _success =
        (
            (rb             = audio_malloc(sizeof(struct ringbuf))) &&
            (buf            = audio_calloc(n_blocks, block_size))   &&
            (rb->can_read   = xSemaphoreCreateBinary())             &&
            (rb->lock       = xSemaphoreCreateMutex())              &&
            (rb->can_write  = xSemaphoreCreateBinary())
        );

    AUDIO_MEM_CHECK(TAG, _success, goto _rb_init_failed);

    rb->p_o = rb->p_r = rb->p_w = buf;
    rb->fill_cnt = 0;
    rb->size = block_size * n_blocks;
    rb->is_done_write = false;
    rb->unblock_reader_flag = false;
    rb->abort_read = false;
    rb->abort_write = false;
    return rb;
_rb_init_failed:
    rb_destroy(rb);
    return NULL;
}

esp_err_t rb_destroy(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (rb->p_o) {
        audio_free(rb->p_o);
        rb->p_o = NULL;
    }
    if (rb->can_read) {
        vSemaphoreDelete(rb->can_read);
        rb->can_read = NULL;
    }
    if (rb->can_write) {
        vSemaphoreDelete(rb->can_write);
        rb->can_write = NULL;
    }
    if (rb->lock) {
        vSemaphoreDelete(rb->lock);
        rb->lock = NULL;
    }
    audio_free(rb);
    rb = NULL;
    return ESP_OK;
}

esp_err_t rb_reset(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_FAIL;
    }
    rb->p_r = rb->p_w = rb->p_o;
    rb->fill_cnt = 0;
    rb->is_done_write = false;

    rb->unblock_reader_flag = false;
    rb->abort_read = false;
    rb->abort_write = false;
    return ESP_OK;
}

int rb_bytes_available(ringbuf_handle_t rb)
{
    return (rb->size - rb->fill_cnt);
}

int rb_bytes_filled(ringbuf_handle_t rb)
{
    if (rb) {
        return rb->fill_cnt;
    }
    return ESP_FAIL;
}

static void rb_release(SemaphoreHandle_t handle)
{
    xSemaphoreGive(handle);
}

#define rb_block(handle, time) xSemaphoreTake(handle, time)

int rb_read(ringbuf_handle_t rb, char *buf, int buf_len, TickType_t ticks_to_wait)
{
    int read_size = 0;
    int total_read_size = 0;
    int ret_val = 0;

    if (rb == NULL) {
        return RB_FAIL;
    }

    while (buf_len) {
        //take buffer lock
        if (rb_block(rb->lock, portMAX_DELAY) != pdTRUE) {
            ret_val = RB_TIMEOUT;
            goto read_err;
        }

        if (rb->fill_cnt < buf_len) {
            read_size = rb->fill_cnt;
            /**
             * When non-multiple of 4(word size) bytes are written to I2S, there is noise.
             * Below is the kind of workaround to read only in multiple of 4. Avoids noise when rb is read in small chunks.
             * Note that, when we have buf_len bytes available in rb, we still read those irrespective of if it's multiple of 4.
             */
            read_size = read_size & 0xfffffffc;
            if ((read_size == 0) && rb->is_done_write) {
                read_size = rb->fill_cnt;
            }
        } else {
            read_size = buf_len;
        }

        if (read_size == 0) {
            //no data to read, release thread block to allow other threads to write data

            if (rb->is_done_write) {
                ret_val = RB_DONE;
                rb_release(rb->lock);
                goto read_err;
            }
            if (rb->abort_read) {
                ret_val = RB_ABORT;
                rb_release(rb->lock);
                goto read_err;
            }
            if (rb->unblock_reader_flag) {
                //reader_unblock is nothing but forced timeout
                ret_val = RB_TIMEOUT;
                rb_release(rb->lock);
                goto read_err;
            }

            rb_release(rb->lock);
            rb_release(rb->can_write);
            //wait till some data available to read
            if (rb_block(rb->can_read, ticks_to_wait) != pdTRUE) {
                ret_val = RB_TIMEOUT;
                goto read_err;
            }
            continue;
        }

        if ((rb->p_r + read_size) > (rb->p_o + rb->size)) {
            int rlen1 = rb->p_o + rb->size - rb->p_r;
            int rlen2 = read_size - rlen1;
            if (buf) {
                memcpy(buf, rb->p_r, rlen1);
                memcpy(buf + rlen1, rb->p_o, rlen2);
            }
            rb->p_r = rb->p_o + rlen2;
        } else {
            if (buf) {
                memcpy(buf, rb->p_r, read_size);
            }
            rb->p_r = rb->p_r + read_size;
        }

        buf_len -= read_size;
        rb->fill_cnt -= read_size;
        total_read_size += read_size;
        buf += read_size;
        rb_release(rb->lock);
        if (buf_len == 0) {
            break;
        }
    }
read_err:
    if (total_read_size > 0) {
        rb_release(rb->can_write);
    }
    if ((ret_val == RB_FAIL) ||
        (ret_val == RB_ABORT)) {
        total_read_size = ret_val;
    }
    rb->unblock_reader_flag = false; /* We are anyway unblocking the reader */
    return total_read_size > 0 ? total_read_size : ret_val;
}

int rb_write(ringbuf_handle_t rb, char *buf, int buf_len, TickType_t ticks_to_wait)
{
    int write_size;
    int total_write_size = 0;
    int ret_val = 0;

    if (rb == NULL || buf == NULL) {
        return RB_FAIL;
    }

    while (buf_len) {
        //take buffer lock
        if (rb_block(rb->lock, portMAX_DELAY) != pdTRUE) {
            ret_val =  RB_TIMEOUT;
            goto write_err;
        }
        write_size = rb_bytes_available(rb);

        if (buf_len < write_size) {
            write_size = buf_len;
        }

        if (write_size == 0) {
            //no space to write, release thread block to allow other to read data
            if (rb->is_done_write) {
                ret_val = RB_DONE;
                rb_release(rb->lock);
                goto write_err;
            }
            if (rb->abort_write) {
                ret_val = RB_ABORT;
                rb_release(rb->lock);
                goto write_err;
            }

            rb_release(rb->lock);
            rb_release(rb->can_read);
            //wait till we have some empty space to write
            if (rb_block(rb->can_write, ticks_to_wait) != pdTRUE) {
                ret_val = RB_TIMEOUT;
                goto write_err;
            }
            continue;
        }

        if ((rb->p_w + write_size) > (rb->p_o + rb->size)) {
            int wlen1 = rb->p_o + rb->size - rb->p_w;
            int wlen2 = write_size - wlen1;
            memcpy(rb->p_w, buf, wlen1);
            memcpy(rb->p_o, buf + wlen1, wlen2);
            rb->p_w = rb->p_o + wlen2;
        } else {
            memcpy(rb->p_w, buf, write_size);
            rb->p_w = rb->p_w + write_size;
        }

        buf_len -= write_size;
        rb->fill_cnt += write_size;
        total_write_size += write_size;
        buf += write_size;
        rb_release(rb->lock);
        if (buf_len == 0) {
            break;
        }
    }
write_err:
    if (total_write_size > 0) {
        rb_release(rb->can_read);
    }
    if ((ret_val == RB_FAIL) ||
        (ret_val == RB_ABORT)) {
        total_write_size = ret_val;
    }
    return total_write_size > 0 ? total_write_size : ret_val;
}

static esp_err_t rb_abort_read(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    rb->abort_read = true;
    xSemaphoreGive(rb->can_read);
    return ESP_OK;
}

static esp_err_t rb_abort_write(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    rb->abort_write = true;
    xSemaphoreGive(rb->can_write);
    return ESP_OK;
}

esp_err_t rb_abort(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = rb_abort_read(rb);
    err |= rb_abort_write(rb);
    return err;
}

bool rb_is_full(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return false;
    }
    return (rb->size == rb->fill_cnt);
}

esp_err_t rb_done_write(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    rb->is_done_write = true;
    rb_release(rb->can_read);
    return ESP_OK;
}

esp_err_t rb_unblock_reader(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    rb->unblock_reader_flag = true;
    rb_release(rb->can_read);
    return ESP_OK;
}

bool rb_is_done_write(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return false;
    }
    return (rb->is_done_write);
}

int rb_get_size(ringbuf_handle_t rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return rb->size;
}
