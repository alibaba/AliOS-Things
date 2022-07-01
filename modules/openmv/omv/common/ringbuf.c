/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Simple Ring Buffer implementation.
 */
#include <string.h>
#include "ringbuf.h"

void ring_buf_init(ring_buf_t *buf)
{
    memset(buf, 0, sizeof(*buf));
}

int ring_buf_empty(ring_buf_t *buf)
{
    return (buf->head == buf->tail);
}

void ring_buf_put(ring_buf_t *buf, uint8_t c)
{
   if ((buf->tail + 1) % BUFFER_SIZE == buf->head) {
      /*buffer is full*/
      return;
   }

   buf->data[buf->tail] = c;
   buf->tail = (buf->tail + 1) % BUFFER_SIZE;
}

uint8_t ring_buf_get(ring_buf_t *buf)
{
   uint8_t c;
   if (buf->head == buf->tail) {
       /*buffer is empty*/
       return 0;
   }

   c = buf->data[buf->head];
   buf->head = (buf->head + 1) % BUFFER_SIZE;
   return c;
}
