/**
 * @file k_buf_queue.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_BUF_QUEUE_H
#define K_BUF_QUEUE_H

/** @addtogroup aos_rhino bitmap
 *  buf_queue passes the specified length information.
 *  When receiving a message, it can cause task blocking.
 *  When sending a messages, it can't cause task blocking.
 *  may discard the messages when the buffer is full.
 *
 *  @{
 */

/**
 * buf_que object
 */
typedef struct {
    blk_obj_t    blk_obj;           /**< Manage blocked tasks */
    void        *buf;               /**< ringbuf address */
    k_ringbuf_t  ringbuf;           /**< ringbuf management */
    size_t       max_msg_size;      /**< limited message length */
    size_t       cur_num;           /**< msgs used */
    size_t       peak_num;          /**< maximum msgs used */
    size_t       min_free_buf_size; /**< minimum free size */
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t      buf_queue_item;    /**< kobj list for statistics */
#endif
#if (RHINO_CONFIG_USER_SPACE > 0)
    uint32_t     key;
#endif
    uint8_t      mm_alloc_flag;     /**< buffer from internal malloc or caller input */
} kbuf_queue_t;

/**
 * buf_queue infomation
 */
typedef struct {
    size_t buf_size;                /**< whole queue size */
    size_t max_msg_size;
    size_t cur_num;
    size_t peak_num;
    size_t free_buf_size;
    size_t min_free_buf_size;
} kbuf_queue_info_t;

/**
 * Create a buf_queue.
 *
 * @param[in]  queue    pointer to the queue (the space is provided outside, by user)
 * @param[in]  name     name of the queue
 * @param[in]  buf      pointer to the buf
 * @param[in]  size     size of the buf
 * @param[in]  max_msg  max size of one msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg);

/**
 * Create a fix-msg-len buf_queue.
 *
 * @param[in]  queue     pointer to the queue (the space is provided outside, by user)
 * @param[in]  name      name of the queue
 * @param[in]  buf       pointer to the buf
 * @param[in]  msg_size  fix size of the msg
 * @param[in]  msg_num   number of msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_fix_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                    void *buf, size_t msg_size, size_t msg_num);

/**
 * Delete a buf_queue.
 *
 * @param[in]  queue  pointer to the buf_queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_del(kbuf_queue_t *queue);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

/**
 * Malloc and create a buf_queue.
 *
 * @param[out]  queue    pointer to the queue (the space is provided inside, from heap)
 * @param[in]   name     pointer to the nam
 * @param[in]   size     size of the buf
 * @param[in]   max_msg  max size of one msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                    size_t size, size_t max_msg);

/**
 * Malloc and create a fix-msg-len buf_queue.
 *
 * @param[out] queue     pointer to the queue (the space is provided inside, from heap)
 * @param[in]  name      name of the queue
 * @param[in]  buf       pointer to the buf
 * @param[in]  msg_size  size of the msg
 * @param[in]  msg_num   number of msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_fix_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                        size_t msg_size, size_t msg_num);

/**
 * Delete and free a buf_queue.
 *
 * @param[in]  queue  pointer to the queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue);
#endif

/**
 * This function will send a msg at the end of queue.
 * If queue is full, task is non-blocking, return fail.
 *
 * @param[in]  queue  pointer to the queue
 * @param[in]  msg    pointer to msg to be send
 * @param[in]  size   size of the msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size);

/**
 * This function will receive msg form aqueue.
 * If queue is empty, task is blocking.
 *
 * @param[in]   queue  pointer to the queue
 * @param[in]   ticks  ticks to wait before receiving msg
 * @param[out]  msg    pointer to the buf to save msg
 * @param[out]  size   size of received msg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks, void *msg, size_t *size);

/**
 * Reset queue, abondon the msg.
 *
 * @param[in]  queue  pointer to the queue
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue);

/**
 * Get information of a queue.
 *
 * @param[in]   queue  pointer to the queue
 * @param[out]  info   info msg of the queue buf
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_info_get(kbuf_queue_t *queue, kbuf_queue_info_t *info);

/** @} */

#endif /* K_BUF_QUEUE_H */

