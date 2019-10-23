/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"

#include <stdio.h>

#include "k_api.h"
#include "res.h"

void* res_malloc(int pid, size_t len)
{
    task_group_t      *group;
    res_request_msg_t  request;
    res_reply_msg_t    reply;
    size_t             reply_size;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return NULL;
    }

    memset(&request, 0, sizeof(res_request_msg_t));
    request.type    = REQUEST_MALLOC;
    request.arg_cnt = 1;
    request.has_ret = 1;
    request.arg[0]  = (void*)len;

    krhino_mutex_lock(&group->res_mutex, RHINO_WAIT_FOREVER);
    krhino_buf_queue_send(group->res_request_q, &request, sizeof(res_request_msg_t));
    memset(&reply, 0, sizeof(res_reply_msg_t));
    krhino_buf_queue_recv(group->res_reply_q, RHINO_WAIT_FOREVER, &reply, &reply_size);
    krhino_mutex_unlock(&group->res_mutex);

    return reply.ret_val;
}

void res_free(int pid, void *ptr)
{
    task_group_t      *group;
    res_request_msg_t  request;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return;
    }

    memset(&request, 0, sizeof(res_request_msg_t));
    request.type    = REQUEST_FREE;
    request.arg_cnt = 1;
    request.has_ret = 0;
    request.arg[0]  = ptr;

    krhino_buf_queue_send(group->res_request_q, &request, sizeof(res_request_msg_t));
}

int res_init(task_group_t *group)
{
    kbuf_queue_t *res_request_q;
    kbuf_queue_t *res_reply_q;

    kstat_t stat;

    stat = krhino_fix_buf_queue_dyn_create(&res_request_q,
                                    "res_request_buf_queue",
                                    sizeof(res_request_msg_t),
                                    RES_QUEUE_MSG_NUM);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    group->res_request_q = res_request_q;

    stat = krhino_fix_buf_queue_dyn_create(&res_reply_q,
                                    "res_reply_buf_queue",
                                    sizeof(res_reply_msg_t),
                                    RES_QUEUE_MSG_NUM);
    if (stat != RHINO_SUCCESS) {
        krhino_buf_queue_dyn_del(res_reply_q);
        group->res_request_q = NULL;
        return -2;
    }

    group->res_reply_q = res_reply_q;

    krhino_mutex_create(&group->res_mutex, "res_mutex");

    return 0;
}

void res_exit(task_group_t *group)
{
    if (group->res_request_q) {
        krhino_buf_queue_dyn_del(group->res_request_q);
    }

    if (group->res_reply_q) {
        krhino_buf_queue_dyn_del(group->res_reply_q);
    }

    krhino_mutex_del(&group->res_mutex);
}

