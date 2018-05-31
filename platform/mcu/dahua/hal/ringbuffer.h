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
#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t  *buffer;
    uint32_t length;
    uint32_t head;
    uint32_t  tail;
} ringbuffer_t;

/**
 * @brief  create a ring buffer
 * @param[in] length length space of the ring buffer
 * @return  pointer to ring buffer
 */
ringbuffer_t *ringbuffer_create(int length);

/**
 * @brief   destroy the ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  null
 */
void ringbuffer_destroy(ringbuffer_t * buffer);

/**
 * @brief   read data from ring buffer.
 * @param[in] buffer pointer to the ring buffer
 * @param[in] target pointer to buffer for data to read from ring buffer
 * @param[in] amount amount of data items to read
 * @return  number of actual read data
 */
int ringbuffer_read(ringbuffer_t *buffer, uint8_t *target, uint32_t amount);

/**
 * @brief   write data to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @param[in] data pointer to buffer for data to write to ring buffer
 * @param[in] length length of data items to write
 * @return  0 for success
 */
int ringbuffer_write(ringbuffer_t * buffer, uint8_t *data, uint32_t length);

/**
 * @brief   is the ring buffer empty?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int ringbuffer_empty(ringbuffer_t * buffer);
/**
 * @brief   is the ring buffer full?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int ringbuffer_full(ringbuffer_t * buffer);
/**
 * @brief   available write space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return   number of write spcae
 */
int ringbuffer_available_write_space(ringbuffer_t * buffer);
/**
 * @brief    available read space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  number of read spcae
 */
int ringbuffer_available_read_space(ringbuffer_t * buffer);

#define ringbuffer_available_write_space(B) (\
        (B)->length - ringbuffer_available_read_space(B))

#define ringbuffer_full(B) (ringbuffer_available_write_space(B) == 0)

#define ringbuffer_empty(B) (ringbuffer_available_read_space((B)) == 0)

#define ringbuffer_clear(B) (B->tail = B->head = 0)
