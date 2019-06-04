/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_UCALLBACK > 0)

#include "k_api.h"
#include "callback.h"

#define CB_BUF_QUEUE_MSG_NUM (3)

kstat_t cb_call_buf_queue_push(kbuf_queue_t *buf_queue, cb_call_msg_t *msg)
{
    return krhino_buf_queue_send(buf_queue, msg, sizeof(cb_call_msg_t));
}

kstat_t cb_ret_buf_queue_pop(kbuf_queue_t *buf_queue, cb_ret_msg_t *msg, size_t *size)
{
    return krhino_buf_queue_recv(buf_queue, RHINO_WAIT_FOREVER, msg, size);
}

int cb_init(task_group_t *group)
{
    kstat_t       ret;
    kbuf_queue_t *cb_call_buf_queue;
    kbuf_queue_t *cb_ret_buf_queue;

    NULL_PARA_CHK(group);

    /* 1. allocate cb_ret_buf_queue */
    ret = krhino_fix_buf_queue_dyn_create(&cb_ret_buf_queue,
                                          "cb_ret_buf_queue",
                                          sizeof(cb_ret_msg_t),
                                          CB_BUF_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        return -1;
    }

    group->cb_ret_buf_queue = cb_ret_buf_queue;

    /* 2. allocate cb_call_buf_queue */
    ret = krhino_fix_buf_queue_dyn_create(&cb_call_buf_queue,
                                          "cb_call_buf_queue",
                                          sizeof(cb_call_msg_t),
                                          CB_BUF_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        krhino_buf_queue_dyn_del(cb_ret_buf_queue);
        group->cb_ret_buf_queue = NULL;
        return -2;
    }

    group->cb_call_buf_queue = cb_call_buf_queue;

    return 0;
}

/** Brief: delete callback queue and callback buf queue.
 *         This function should be called when process exits
 */
void cb_destory(task_group_t *group)
{
    if (group->cb_ret_buf_queue) {
        krhino_buf_queue_dyn_del(group->cb_ret_buf_queue);
        group->cb_ret_buf_queue = NULL;
    }

    if (group->cb_call_buf_queue) {
        krhino_buf_queue_dyn_del(group->cb_call_buf_queue);
        group->cb_call_buf_queue = NULL;
    }
}

#endif /* RHINO_CONFIG_UCALLBACK */

