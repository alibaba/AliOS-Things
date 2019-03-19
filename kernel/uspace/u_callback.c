/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <u_callback.h>
#include <u_task.h>
#include <u_res.h>

//#define CALLBACK_DEBUG

#ifdef CALLBACK_DEBUG
#define CB_DEBUG(fmt, arg...) printf(fmt, ##arg)
#else
#define CB_DEBUG(fmt, arg...)
#endif

#define CB_BUF_QUEUE_MSG_NUM    (32)

int32_t get_cb_queue_id(void)
{
    ktask_t *task;

    task = krhino_cur_task_get();
    if (NULL == task) {
        return -1;
    }

    return task->pid;
}

kstat_t cb_call_buf_queue_push(kbuf_queue_t *buf_queue, cb_call_msg_t *msg)
{
    return krhino_buf_queue_send(buf_queue, msg, sizeof(cb_call_msg_t));
}

kstat_t cb_ret_buf_queue_pop(kbuf_queue_t *buf_queue, cb_ret_msg_t *msg, size_t *size)
{
    return krhino_buf_queue_recv(buf_queue, RHINO_WAIT_FOREVER, msg, size);
}

kstat_t create_cb_buf_queues(ktask_t *task)
{
    kstat_t       ret;
    kbuf_queue_t *cb_call_buf_queue;
    kbuf_queue_t *cb_ret_buf_queue;
    kbuf_queue_t *resv_cb_call_buf_queue;
    kbuf_queue_t *resv_cb_ret_buf_queue;
    task_group_t *group;

    NULL_PARA_CHK(task);

    group = task->task_group;
    NULL_PARA_CHK(group);

    /* 1. allocate cb_ret_buf_queue */
    ret = krhino_fix_buf_queue_dyn_create(&cb_ret_buf_queue,
                                          "cb_ret_buf_queue",
                                          sizeof(cb_ret_msg_t),
                                          CB_BUF_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        CB_DEBUG("%s: create cb_ret_buf_queue failed, ret %d\r\n",
                 __func__, ret);
        return ret;
    }

    CB_DEBUG("%s: cb_ret_buf_queue: %p\r\n", __func__, cb_ret_buf_queue);
    group->cb_ret_buf_queue = cb_ret_buf_queue;

    /* 2. allocate cb_call_buf_queue */
    ret = krhino_fix_buf_queue_dyn_create(&cb_call_buf_queue,
                                          "cb_call_buf_queue",
                                          sizeof(cb_call_msg_t),
                                          CB_BUF_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        CB_DEBUG("%s: create cb_call_buf_queue failed, ret %d\r\n",
                 __func__, ret);
        goto err;
    }

    CB_DEBUG("%s: cb_call_buf_queue: %p\r\n", __func__, cb_call_buf_queue);
    group->cb_call_buf_queue = cb_call_buf_queue;

    return RHINO_SUCCESS;

err:
    krhino_buf_queue_dyn_del(cb_ret_buf_queue);
    group->cb_ret_buf_queue = NULL;

    return ret;
}

/** Brief: delete callback queue and callback buf queue.
 *         This function should be called when process exits
 *  1. delete all items in queue
 *  2. delete the queue
 *  3. delete the buf_queue
 */
kstat_t delete_cb_buf_queues(task_group_t *group)
{
    kstat_t       ret;
    kbuf_queue_t *cb_ret_buf_queue;
    kbuf_queue_t *cb_call_buf_queue;

    CPSR_ALLOC();

    NULL_PARA_CHK(group);

    /* free cb_ret_buf_queue */
    RHINO_CRITICAL_ENTER();
    cb_ret_buf_queue = group->cb_ret_buf_queue;
    group->cb_ret_buf_queue = NULL;
    RHINO_CRITICAL_EXIT();

    NULL_PARA_CHK(cb_ret_buf_queue);
    task_group_remove_res_obj(group, RES_OBJ_BUF_QUEUE, cb_ret_buf_queue);
    krhino_buf_queue_dyn_del(cb_ret_buf_queue);

    /* free cb_call_buf_queue */
    RHINO_CRITICAL_ENTER();
    cb_call_buf_queue = group->cb_call_buf_queue;
    group->cb_call_buf_queue = NULL;
    RHINO_CRITICAL_EXIT();

    NULL_PARA_CHK(cb_call_buf_queue);
    task_group_remove_res_obj(group, RES_OBJ_BUF_QUEUE, cb_call_buf_queue);
    krhino_buf_queue_dyn_del(cb_call_buf_queue);

    return RHINO_SUCCESS;
}

