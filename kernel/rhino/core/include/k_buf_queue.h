/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_BUF_QUEUE_H
#define K_BUF_QUEUE_H

typedef struct {
    blk_obj_t      blk_obj;
    void          *buf;
    k_ringbuf_t    ringbuf;
    size_t         max_msg_size;
    size_t         cur_num;
    size_t         peak_num;
    size_t         min_free_buf_size;
#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_t        buf_queue_item;
#endif
    uint8_t        mm_alloc_flag;
} kbuf_queue_t;

typedef struct {
    size_t buf_size;
    size_t max_msg_size;
    size_t cur_num;
    size_t peak_num;
    size_t free_buf_size;
    size_t min_free_buf_size;
} kbuf_queue_info_t;

/**
 * This function will create a buf-queue
 * @param[in]  queue    pointer to the queue(the space is provided by user)
 * @param[in]  name     name of the queue
 * @param[in]  buf      pointer to the buf
 * @param[in]  size     size of the buf
 * @param[in]  max_msg  max size of one msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf,
                                size_t size, size_t max_msg);

/**
 * This function will create a fix buf-queue
 * @param[in]  queue    pointer to the queue(the space is provided by user)
 * @param[in]  name     name of the queue
 * @param[in]  buf      pointer to the buf
 * @param[in]  msg_size size of the msg
 * @param[in]  msg_num  number of msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_fix_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                    void *buf, size_t msg_size, size_t msg_num);

/**
 * This function will delete a queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_del(kbuf_queue_t *queue);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a dyn-queue
 * @param[out]  queue    pointer to the queue(The space is provided by kernel)
 * @param[in]   name     pointer to the nam
 * @param[in]   size     size of the buf
 * @param[in]   max_msg  max size of one msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                    size_t size, size_t max_msg);

/**
 * This function will delete a dyn-queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue);
#endif

/**
 * This function will send a msg at the end of queue
 * @param[in]  queue  pointer to the queue
 * @param[in]  msg    pointer to msg to be send
 * @param[in]  size   size of the msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size);


/**
 * This function will receive msg form aqueue
 * @param[in]   queue  pointer to the queue
 * @param[in]   ticks  ticks to wait before receiving msg
 * @param[out]  msg    pointer to the buf to save msg
 * @param[out]  size   size of received msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks, void *msg,
                              size_t *size);

/**
 * This function will reset queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue);

/**
 * This function will get information of a queue
 * @param[in]   queue  pointer to the queue
 * @param[out]  free   free size of the queue buf
 * @param[out]  total  total size of the queue buf
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_buf_queue_info_get(kbuf_queue_t *queue, kbuf_queue_info_t *info);

#endif /* K_BUF_QUEUE_H */

