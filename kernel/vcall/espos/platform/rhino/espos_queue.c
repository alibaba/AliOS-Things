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
#include "k_api.h"
#include "espos_err.h"
#include "espos_queue.h"
#include "espos_scheduler.h"

/**
 * @brief create a queue
 */
esp_err_t espos_queue_create (
    espos_queue_t *queue,
    espos_size_t msg_len,
    espos_size_t queue_len
)
{
    kstat_t ret;
    kbuf_queue_t **pqueue = (kbuf_queue_t **)queue;

    ret = krhino_buf_queue_dyn_create(pqueue, "default_queue", queue_len * (msg_len + COMPRESS_LEN(msg_len)), msg_len);

    return espos_err_map(ret);
}

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
esp_err_t espos_queue_set_name(espos_queue_t queue, const char *name)
{
    return 0;
}

/**
 * @brief send a message to the queue
 */
esp_err_t IRAM_ATTR espos_queue_send_generic (
    espos_queue_t queue,
    void *msg,
    espos_tick_t wait_ticks,
    espos_pos_t pos,
    espos_opt_t opt
)
{
    kstat_t ret;
    kbuf_queue_t *pqueue = (kbuf_queue_t *)queue;

    ret = krhino_buf_queue_send(pqueue, msg, pqueue->max_msg_size);

    return espos_err_map(ret);
}

/**
 * @brief receive a message of the queue
 */
esp_err_t espos_queue_recv_generic (
    espos_queue_t queue,
    void *msg,
    espos_tick_t wait_ticks,
    espos_opt_t opt
)
{
    kstat_t ret;
    kbuf_queue_t *pqueue = (kbuf_queue_t *)queue;
    size_t msg_len;

    ret = krhino_buf_queue_recv(pqueue, wait_ticks, msg, &msg_len);

    return espos_err_map(ret);

}

/**
 * @brief get current message number of the queue
 */
espos_size_t espos_queue_msg_waiting(espos_queue_t queue)
{
    CPSR_ALLOC();
    espos_size_t item_size;

    kbuf_queue_t *pqueue = (kbuf_queue_t *)queue;
    
    RHINO_CRITICAL_ENTER();

    //item_size = (pqueue->buf_size - pqueue->free_buf_size) / (pqueue->max_msg_size +  HEADER_SIZE);
    item_size = pqueue->cur_num;
    RHINO_CRITICAL_EXIT();

    return item_size;
}

/**
 * @brief reset the queue
 */
esp_err_t espos_queue_flush (
    espos_queue_t queue
)
{
    kstat_t ret;
    kbuf_queue_t *pqueue = (kbuf_queue_t *)queue;

    ret = krhino_buf_queue_flush(pqueue);

    return espos_err_map(ret);
}

/**
 * @brief delete the queue
 */
esp_err_t espos_queue_del (
    espos_queue_t queue
)
{
    kstat_t ret;
    kbuf_queue_t *pqueue = (kbuf_queue_t *)queue;

    ret = krhino_buf_queue_dyn_del(pqueue);

    return espos_err_map(ret);
}

