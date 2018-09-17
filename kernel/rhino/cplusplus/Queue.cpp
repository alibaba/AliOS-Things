/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "Queue.h"

using namespace AliOS;

/**
 * This function will create a buf-queue
 * @param[in]  name     name of the queue
 * @param[in]  buf      pointer to the buf
 * @param[in]  size     size of the buf
 * @param[in]  max_msg  max size of one msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::create(const name_t *name, void *buf, size_t size, size_t max_msg)
{
    return krhino_buf_queue_create(&_buf_queue_def, name, buf, size, max_msg);
}

/**
 * This function will delete a queue
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::destory(void)
{
    return krhino_buf_queue_del(&_buf_queue_def);
}

/**
 * This function will send a msg at the end of queue
 * @param[in]  msg    pointer to msg to be send
 * @param[in]  size   size of the msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::send(void *msg, size_t size)
{
    return krhino_buf_queue_send(&_buf_queue_def, msg, size);
}

/**
 * This function will receive msg form aqueue
 * @param[in]   ticks  ticks to wait before receiving msg
 * @param[out]  msg    pointer to the buf to save msg
 * @param[out]  size   size of received msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::receive(void *msg, size_t *size, uint32_t millisec)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == Queue_WAIT_FOREVER){
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(millisec);
    }

    return krhino_buf_queue_recv(&_buf_queue_def, ticks, msg, size);
}

/**
 * This function will reset queue
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::flush(void)
{
    return krhino_buf_queue_flush(&_buf_queue_def);
}

/**
 * This function will get information of a queue
 * @param[out]  free   free size of the queue buf
 * @param[out]  total  total size of the queue buf
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Queue::info_get(kbuf_queue_info_t *info)
{
    return krhino_buf_queue_info_get(&_buf_queue_def, info);
}

/**
 * This function will get a Queue struct pointer
 * @param[in]  none
 * @return  Queue struct pointer
 */
kbuf_queue_t *Queue::self(void)
{
    return &_buf_queue_def;
}
