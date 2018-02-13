/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include "ringbuffer.h"

#define MIN(a, b) (a)<(b)? (a) : (b)

ringbuffer_t *ringbuffer_create(int length)
{
    ringbuffer_t *buffer = aos_malloc(sizeof(ringbuffer_t));

    if (buffer == NULL) {
        return NULL;
    }

    buffer->length = length;
    buffer->head = 0;
    buffer->tail = 0;
    /* tail cannot save data, must extend one byte */
    buffer->buffer = aos_malloc(buffer->length + 1);

    return buffer;
}

void ringbuffer_destroy(ringbuffer_t *buffer)
{
    if (buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

int ringbuffer_available_read_space(ringbuffer_t *buffer)
{
    if (buffer->head == buffer->tail) {
        return 0;
    } else if (buffer->head < buffer->tail) {
        return buffer->tail - buffer->head;
    } else {
        return buffer->length - (buffer->head - buffer->tail);
    }
}

int ringbuffer_write(ringbuffer_t *buffer, uint8_t *data, uint32_t length)
{
    int i = 0;

    if (buffer == NULL || data == NULL || length == 0) {
        return -1;
    }

    /* if empty reset head and tail to zero */
    if (ringbuffer_empty(buffer)) {
        ringbuffer_clear(buffer);
    }

    for (i = 0; i < length; i++) {

        if (ringbuffer_full(buffer)) {
            printf("ringbuffer full\n");
            break;
        }

        buffer->buffer[buffer->tail] = data[i];

        buffer->tail++;
        buffer->tail %= (buffer->length + 1);

    }

    /* return real write len */
    return i;
}

int ringbuffer_read(ringbuffer_t *buffer, uint8_t *target, uint32_t amount)
{
    int copy_sz = 0;
    int i;

    if (buffer == NULL || target == NULL || amount == 0) {
        return -1;
    }

    if (ringbuffer_empty(buffer)) {
        return 0;
    }

    /* get real read size */
    copy_sz = MIN(amount, ringbuffer_available_read_space(buffer));

    /* cp data to user buffer */
    for (i = 0; i < copy_sz; i++) {
        target[i] = buffer->buffer[buffer->head];

        buffer->head++;
        buffer->head %= (buffer->length + 1);
    }

    return copy_sz;
}
