/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/** \ingroup Group_Common
 * \defgroup group_fifo fifo
 *  @{ */

#ifndef _TUSB_FIFO_H_
#define _TUSB_FIFO_H_

// mutex is only needed for RTOS
// for OS None, we don't get preempted
#define CFG_FIFO_MUTEX      (CFG_TUSB_OS != OPT_OS_NONE)

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#if CFG_FIFO_MUTEX
#define tu_fifo_mutex_t  osal_mutex_t
#endif


/** \struct tu_fifo_t
 * \brief Simple Circular FIFO
 */
typedef struct
{
           uint8_t* buffer    ; ///< buffer pointer
           uint16_t depth     ; ///< max items
           uint16_t item_size ; ///< size of each item
           bool overwritable  ;

  volatile uint16_t count     ; ///< number of items in queue
  volatile uint16_t wr_idx    ; ///< write pointer
  volatile uint16_t rd_idx    ; ///< read pointer

#if CFG_FIFO_MUTEX
  tu_fifo_mutex_t mutex;
#endif

} tu_fifo_t;

#define TU_FIFO_DEF(_name, _depth, _type, _overwritable) \
  uint8_t _name##_buf[_depth*sizeof(_type)]; \
  tu_fifo_t _name = {                        \
      .buffer       = _name##_buf,           \
      .depth        = _depth,                \
      .item_size    = sizeof(_type),         \
      .overwritable = _overwritable,         \
  }

bool tu_fifo_clear(tu_fifo_t *f);
bool tu_fifo_config(tu_fifo_t *f, void* buffer, uint16_t depth, uint16_t item_size, bool overwritable);

#if CFG_FIFO_MUTEX
static inline void tu_fifo_config_mutex(tu_fifo_t *f, tu_fifo_mutex_t mutex_hdl)
{
  f->mutex = mutex_hdl;
}
#endif

bool     tu_fifo_write   (tu_fifo_t* f, void const * p_data);
uint16_t tu_fifo_write_n (tu_fifo_t* f, void const * p_data, uint16_t count);

bool     tu_fifo_read    (tu_fifo_t* f, void * p_buffer);
uint16_t tu_fifo_read_n  (tu_fifo_t* f, void * p_buffer, uint16_t count);

bool     tu_fifo_peek_at (tu_fifo_t* f, uint16_t pos, void * p_buffer);

static inline bool tu_fifo_peek(tu_fifo_t* f, void * p_buffer)
{
  return tu_fifo_peek_at(f, 0, p_buffer);
}

static inline bool tu_fifo_empty(tu_fifo_t* f)
{
  return (f->count == 0);
}

static inline bool tu_fifo_full(tu_fifo_t* f)
{
  return (f->count == f->depth);
}

static inline uint16_t tu_fifo_count(tu_fifo_t* f)
{
  return f->count;
}

static inline uint16_t tu_fifo_remaining(tu_fifo_t* f)
{
  return f->depth - f->count;
}

static inline uint16_t tu_fifo_depth(tu_fifo_t* f)
{
  return f->depth;
}

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_FIFO_H_ */
