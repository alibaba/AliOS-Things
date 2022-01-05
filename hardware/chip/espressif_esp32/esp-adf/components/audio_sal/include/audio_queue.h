/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef __AUDIO_QUEUE_H__
#define __AUDIO_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* audio_queue_handle_t;

/**
 * @brief       Allocate queue instance with given item size and length
 *
 * @param       queue_len       The maximum number of items that the queue can contain
 * @param       item_size       The number of bytes each item in the queue will require
 *
 * @return      - Others:       Queue handle is returned
 *              - NULL:         Failed to create queue handle
 */
audio_queue_handle_t audio_queue_create(uint32_t queue_len, uint32_t item_size);

/**
 * @brief       Delete the queue
 *
 * @param       queue        A pointer to queue handle
 *
 * @return      - 0:         The queue deleted successfully
 */
int audio_queue_delete(audio_queue_handle_t queue);

/**
 * @brief       Post an item on given queue
 *
 * @param       queue            A pointer to queue handle
 * @param       item             A pointer to the item that is to be placed on the queue
 * @param       block_time_ms    The maximum amount of millisecond the task should block waiting for space
 *                               to become available on the queue, should it already be full
 *
 * @return      - 0:             The item was successfully posted
 *              - Otherwise:     Queue full error
 */
int audio_queue_send(audio_queue_handle_t queue, void *item, uint32_t block_time_ms);

/**
 * @brief       Post an item to the front of a queue
 *
 * @param       queue            A pointer to queue handle
 * @param       item             A pointer to the item that is to be placed on the queue
 * @param       block_time_ms    The maximum amount of millisecond the task should block waiting for space
 *                               to become available on the queue, should it already be full
 *
 * @return      - 0:             The item was successfully posted
 *              - Otherwise:     Queue full error
 */
int audio_queue_send_to_front(audio_queue_handle_t queue, void *item, uint32_t block_time_ms);

/**
 * @brief       Receive an item from a queue.
 *
 * @param       queue            A pointer to queue handle
 * @param       item             A pointer to the item that is to be placed on the queue
 * @param       block_time_ms    The maximum amount of millisecond the task should block waiting for space
 *                               to become available on the queue, should it already be full
 *
 * @return      - 0:             An item was successfully received from the queue
 *              - Otherwise:     Failed to received queue
 */
int audio_queue_recv(audio_queue_handle_t queue, void *item, uint32_t block_time_ms);

/**
 * @brief       Return the number of messages stored in a queue.
 *
 * @param       queue            A pointer to queue handle
 *
 * @return      The number of messages available in the queue.
 */
int audio_queue_message_available(const audio_queue_handle_t queue);

/**
 * @brief       Return the number of free spaces available in a queue.
 *
 * @param       queue            A pointer to queue handle
 *
 * @return      The number of spaces available in the queue.
 */
int audio_queue_spaces_available(const audio_queue_handle_t queue);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __AUDIO_QUEUE_H__ */
