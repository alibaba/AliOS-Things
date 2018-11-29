/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_QUEUE_H
#define K_QUEUE_H

#define WAKE_ONE_TASK      0u
#define WAKE_ALL_TASK      1u

typedef struct {
    void  **queue_start;
    size_t  size;
    size_t  cur_num;
    size_t  peak_num;
} msg_q_t;

typedef struct {
    msg_q_t  msg_q;
    klist_t *pend_entry;
} msg_info_t;

typedef struct queue_s {
    blk_obj_t   blk_obj;
    k_ringbuf_t ringbuf;
    msg_q_t     msg_q;
#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_t     queue_item;
#endif
    uint8_t     mm_alloc_flag;
} kqueue_t;

/**
 * This function will create a queue
 * @param[in]  queue    pointer to the queue(the space is provided by user)
 * @param[in]  name     name of the queue
 * @param[in]  start    start address of the queue internal space
 * @param[in]  msg_num  num of the msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_create(kqueue_t *queue, const name_t *name, void **start,
                            size_t msg_num);

/**
 * This function will delete a queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_del(kqueue_t *queue);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a dyn queue
 * @param[in]  queue    pointer to the queue
 * @param[in]  name     name of the queue
 * @param[in]  msg_num  num of the msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_dyn_create(kqueue_t **queue, const name_t *name,
                                size_t msg_num);

/**
 * This function will delete a dyn created queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_dyn_del(kqueue_t *queue);
#endif

/**
 * This function will send a msg to the back of a queue
 * @param[in]  queue  pointer to the queue
 * @param[in]  msg    msg to send
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_back_send(kqueue_t *queue, void *msg);

/**
 * This function will send a msg to a queue and wake all tasks
 * @param[in]  queue  pointer to the queue
 * @param[in]  msg    msg to send
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_all_send(kqueue_t *queue, void *msg);

/**
 * This function will receive msg from a queue
 * @param[in]   queue  pointer to the queue
 * @param[in]   ticks  ticks to wait before receive
 * @param[out]  msg    buf to save msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_recv(kqueue_t *queue, tick_t ticks, void **msg);

/**
 * This function will detect a queue full or not
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_QUEUE_FULL/RHINO_QUEUE_NOT_FULL
 */
kstat_t krhino_queue_is_full(kqueue_t *queue);

/**
 * This function will reset a queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_flush(kqueue_t *queue);

/**
 * This function will get information of a queue
 * @param[in]   queue  pointer to the queue
 * @param[out]  info   buf to save msg-info
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_queue_info_get(kqueue_t *queue, msg_info_t *info);

#endif /* K_QUEUE_H */

