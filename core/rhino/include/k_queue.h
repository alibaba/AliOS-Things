/**
 * @file k_queue.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_QUEUE_H
#define K_QUEUE_H

/** @addtogroup aos_rhino queue
 *  Queue can be used for synchronous communication between tasks.
 *
 *  @{
 */

#define WAKE_ONE_TASK 0u
#define WAKE_ALL_TASK 1u

/**
 * queue statistcs
 */
typedef struct {
    void   **queue_start;
    size_t   size;
    size_t   cur_num;
    size_t   peak_num;
} msg_q_t;

/**
 * queue information
 */
typedef struct {
    msg_q_t  msg_q;
    klist_t *pend_entry;    /**< first pending task which is waiting for msg */
} msg_info_t;

/**
 * mutex object
 */
typedef struct queue_s {
    blk_obj_t   blk_obj;    /**< Manage blocked tasks */
    k_ringbuf_t ringbuf;    /**< Msg passing by ringbuf */
    msg_q_t     msg_q;
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t     queue_item; /**< kobj list for statistics */
#endif
    uint8_t     mm_alloc_flag;  /**< buffer from internal malloc or caller input */
} kqueue_t;

/**
 * Create a queue.
 *
 * @param[in]  queue    pointer to the queue (the space is provided outside, by user)
 * @param[in]  name     name of the queue
 * @param[in]  start    start address of the queue internal space
 * @param[in]  msg_num  num of the msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_create(kqueue_t *queue, const name_t *name, void **start, size_t msg_num);

/**
 * Delete a queue.
 *
 * @param[in]  queue  pointer to the queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_del(kqueue_t *queue);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Malloc and create a queue.
 *
 * @param[out] queue    pointer to the queue (the space is provided inside, from heap)
 * @param[in]  name     name of the queue
 * @param[in]  msg_num  num of the msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_dyn_create(kqueue_t **queue, const name_t *name, size_t msg_num);

/**
 * Delete and free a queue.
 *
 * @param[in]  queue    pointer to the queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_dyn_del(kqueue_t *queue);
#endif

/**
 * Send a message to the tail of a queue.
 *
 * @param[in]  queue  pointer to the queue
 * @param[in]  msg    msg to send
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_back_send(kqueue_t *queue, void *msg);

/**
 * Send a message to the tail of a queue and wake all pending tasks.
 *
 * @param[in]  queue    pointer to the queue
 * @param[in]  msg      msg to send
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_all_send(kqueue_t *queue, void *msg);

/**
 * Receive message from a queue, task may be blocked.
 *
 * @param[in]   queue   pointer to the queue
 * @param[in]   ticks   ticks to wait before receive
 * @param[out]  msg     buf to save msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_recv(kqueue_t *queue, tick_t ticks, void **msg);

/**
 * Check a queue full or not.
 *
 * @param[in]  queue    pointer to the queue
 *
 * @return  the operation status, RHINO_QUEUE_FULL/RHINO_QUEUE_NOT_FULL
 */
kstat_t krhino_queue_is_full(kqueue_t *queue);

/**
 * Reset a queue and discard all unreceived messages.
 *
 * @param[in]  queue    pointer to the queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_flush(kqueue_t *queue);

/**
 * Get information of a queue.
 *
 * @param[in]   queue   pointer to the queue
 * @param[out]  info    buf to save msg-info
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_info_get(kqueue_t *queue, msg_info_t *info);

/** @} */

#endif /* K_QUEUE_H */

