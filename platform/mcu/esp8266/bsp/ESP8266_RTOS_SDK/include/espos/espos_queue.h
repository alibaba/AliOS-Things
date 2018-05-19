// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESPOS_QUEUE_H_
#define _ESPOS_QUEUE_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum espos_queue_pos {
    /* send message to the front of the queue */
    ESPOS_QUEUE_SEND_FRONT = 0,

    /* send message to the back of the queue */
    ESPOS_QUEUE_SEND_BACK,

    ESPOS_QUEUE_POS_MAX
} espos_queue_pos_t;

typedef enum espos_queue_send_opt {
    /* send message with normal option */
    ESPOS_QUEUE_SEND_OPT_NORMAL = 0,

    ESPOS_QUEUE_SEND_OPT_MAX
} espos_queue_send_opt_t;

typedef enum espos_queue_recv_opt {
    /* receive message with normal option */
    ESPOS_QUEUE_RECV_OPT_NORMAL = 0,

    ESPOS_QUEUE_RECV_OPT_MAX
} espos_queue_recv_opt_t;

/**
 * @brief create a queue
 *
 * @param queue queue handle point
 * @param msg_len queue internal message length (bytes)
 * @param queue_len queue internal message maximum number
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINTR  : you do this at interrupt state, and it is not supported
 *       -EINVAL : input parameter error
 *
 * @note all input and out message length is fixedly "msg_len"
 */
esp_err_t espos_queue_create(espos_queue_t *queue, espos_size_t msg_len, espos_size_t queue_len);

/**
 * @brief set a queue name
 *
 * @param queue queue handle
 * @param name queue's name
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_queue_set_name(espos_queue_t queue, const char *name);

/**
 * @brief send a message to the queue, it is suggested that you had better not use "espos_queue_send" directly,
 *       please use "espos_queue_send_front" or "espos_queue_send_back"
 *
 * @param queue queue handle
 * @param msg message point
 * @param wait_ticks sleep for system ticks if the queue is full. Otherwise if queue is not full, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY"
 * @param pos position where sending the message
 *               ESPOS_QUEUE_SEND_FRONT : send message to the front of the queue
 *               ESPOS_QUEUE_SEND_BACK  : send message to the back of the queue
 * @param opt sending option
 *               ESPOS_QUEUE_SEND_OPT_NORMAL : wake up blocked task
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and the queue is full
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
esp_err_t espos_queue_send_generic(espos_queue_t queue, void *msg, espos_tick_t wait_ticks, espos_pos_t pos, espos_opt_t opt);

/**
 * @brief send a message to the front of the queue
 *
 * @param q queue handle
 * @param m message point
 * @param t sleep for system ticks if the queue is full. Otherwise if queue is not full, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY"
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and the queue is full
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
#define espos_queue_send_front(q, m, t) espos_queue_send_generic(q, m, t, ESPOS_QUEUE_SEND_FRONT, ESPOS_QUEUE_SEND_OPT_NORMAL)

/**
 * @brief send a message to the back of the queue
 *
 * @param q queue handle
 * @param m message point
 * @param t sleep for system ticks if the queue is full. Otherwise if queue is not full, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY"
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and the queue is full
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
#define espos_queue_send(q, m, t)  espos_queue_send_generic(q, m, t, ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL)

/**
 * @brief receive a message of the queue
 *
 * @param queue queue handle
 * @param msg message point
 * @param wait_ticks sleep for system ticks if the queue is empty. Otherwise if queue is not empty, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY", at CPU ISR mode, it is forced to be 0
 * @param opt queue sending option
 *               ESPOS_QUEUE_RECV_OPT_NORMAL : use wait_ticks to check if it is need be blocked
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and the queue is empty
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
esp_err_t espos_queue_recv_generic(espos_queue_t queue, void *msg, espos_tick_t wait_ticks, espos_opt_t opt);

/**
 * @brief receive a message of the queue with normal option
 *
 * @param queue queue handle
 * @param msg message point
 * @param wait_ticks sleep for system ticks if the queue is empty. Otherwise if queue is not empty, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY", at CPU ISR mode, it is forced to be 0
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and the queue is empty
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
#define espos_queue_recv(q, m, t) espos_queue_recv_generic(q, m, t, ESPOS_QUEUE_RECV_OPT_NORMAL)

/**
 * @brief get current message number of the queue
 *
 * @param queue queue handle
 *
 * @return current message number of the queue
 */
espos_size_t espos_queue_msg_waiting(espos_queue_t queue);

/**
 * @brief reset the queue
 *
 * @param queue queue handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *
 * @note if low-level module is YunOS, the function will not awake the tasks which is blocked at the queue
 */
esp_err_t espos_queue_flush(espos_queue_t queue);

/**
 * @brief delete the queue
 *
 * @param queue queue handle
 *
 * @return the result
 *             0 : successful
 *        -EINTR : you do this at interrupt state, and it is not supported
 *       -EINVAL : input parameter error
 *
 * @note if low-level module is YunOS, this function will awake all task blocked at the mutex
 */
esp_err_t espos_queue_del(espos_queue_t queue);

#ifdef __cplusplus
}
#endif

#endif /* _ESPOS_QUEUE_H_ */
