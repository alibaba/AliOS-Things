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
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
#include <stdint.h>
#define BUFFER_SIZE (1024)

typedef struct ring_buffer {
   volatile uint32_t head;
   volatile uint32_t tail;
   uint8_t data[BUFFER_SIZE];
} ring_buf_t;

void ring_buf_init(ring_buf_t *buf);
int ring_buf_empty(ring_buf_t *buf);
void ring_buf_put(ring_buf_t *buf, uint8_t c);
uint8_t ring_buf_get(ring_buf_t *buf);
#endif /* __RING_BUFFER_H__ */
