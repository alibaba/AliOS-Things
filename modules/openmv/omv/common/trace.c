/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Trace buffer.
 */
#include <stdint.h>
#include "cmsis_gcc.h"
#include "trace.h"

#define TRACEBUF_SIZE   (256)
typedef struct _tracebuf_t {
    uint8_t idx;
    uint8_t buf[TRACEBUF_SIZE];
} tracebuf_t;

static tracebuf_t tracebuf;

void trace_init()
{
    tracebuf.idx = 0;
    for (int i=0; i<TRACEBUF_SIZE; i++) {
        tracebuf.buf[i] = 0;
    }
}

void trace_insert(uint32_t x)
{
    __disable_irq();
    if (tracebuf.idx < TRACEBUF_SIZE) {
        tracebuf.buf[tracebuf.idx++] = x;
    }
    __enable_irq();
}
