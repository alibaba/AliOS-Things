/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include "k_api.h"
#include "callback.h"
#include "aos/kernel.h"


#define CB_BUF_QUEUE_MSG_NUM (30)

int cb_call_buf_queue_push(void *buf_queue, cb_call_msg_t *msg)
{
    aos_hdl_t queue;
    queue.hdl = buf_queue;
    return aos_queue_send(&queue, msg, sizeof(cb_call_msg_t));
}

int cb_ret_buf_queue_pop(void *buf_queue, cb_ret_msg_t *msg, size_t *size)
{
    aos_hdl_t queue;
    queue.hdl = buf_queue;
    return aos_queue_recv(&queue, RHINO_WAIT_FOREVER, msg, size);
}

int cb_init(task_group_t *group)
{
    kstat_t   ret;
    aos_hdl_t cb_call_buf_queue, cb_ret_buf_queue;
    uint32_t pid;
    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};
    NULL_PARA_CHK(group);
    pid = group->pid;

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_CB_RET_BUF_QUEUE, pid);
    snprintf(name2, USER_NAME_MAX, "%s%d", USER_CB_CALL_BUF_QUEUE, pid);

    ret = aos_queue_open(&cb_ret_buf_queue, name1, O_CREAT,
                         (sizeof(cb_ret_msg_t) + sizeof(intptr_t)) * CB_BUF_QUEUE_MSG_NUM, sizeof(cb_ret_msg_t));

    if (ret != 0) {
        return -1;
    }

    ret = aos_queue_open(&cb_call_buf_queue, name2, O_CREAT,
                         (sizeof(cb_call_msg_t) + sizeof(intptr_t)) * CB_BUF_QUEUE_MSG_NUM, sizeof(cb_call_msg_t));
    if (ret != 0) {
        aos_queue_close(&cb_ret_buf_queue);
        aos_queue_unlink(name1);
        return -2;
    }
    group->cb_ret_buf_queue = cb_ret_buf_queue.hdl;
    group->cb_call_buf_queue = cb_call_buf_queue.hdl;
    return 0;
}

/** Brief: delete callback queue and callback buf queue.
 *         This function should be called when process exits
 */
int cb_exit(task_group_t *group)
{
    uint32_t pid;
    NULL_PARA_CHK(group);
    pid = group->pid;
    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};
    aos_hdl_t cb_call_buf_queue, cb_ret_buf_queue;

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_CB_CALL_BUF_QUEUE, pid);
    snprintf(name2, USER_NAME_MAX, "%s%d", USER_CB_RET_BUF_QUEUE, pid);

    cb_ret_buf_queue.hdl = group->cb_ret_buf_queue;
    cb_call_buf_queue.hdl = group->cb_call_buf_queue;

    aos_queue_close(&cb_ret_buf_queue);
    aos_queue_close(&cb_call_buf_queue);
    aos_queue_unlink(name1);
    aos_queue_unlink(name2);

    group->cb_ret_buf_queue = NULL;
    group->cb_call_buf_queue = NULL;
    return 0;
}

