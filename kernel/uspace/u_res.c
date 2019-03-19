/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <u_task.h>
#include <u_res.h>

static kmutex_t res_mutex;

void* u_res_malloc(int pid, size_t len)
{
    task_group_t *group;
    res_request_msg_t request;
    res_reply_msg_t reply;
    size_t reply_size;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
        return NULL;
    }

    memset(&request, 0, sizeof(res_request_msg_t));
    request.magic = RES_MSG_MAGIC;
    request.type = REQUEST_MALLOC;
    request.arg_cnt = 1;
    request.has_ret = 1;
    request.arg[0] = (unsigned int)len;

    krhino_sem_take(&group->res_sem, RHINO_WAIT_FOREVER);
    krhino_buf_queue_send(group->res_request_q, &request, sizeof(res_request_msg_t));
    memset(&reply, 0, sizeof(res_reply_msg_t));
    krhino_buf_queue_recv(group->res_reply_q, RHINO_WAIT_FOREVER, &reply, &reply_size);
    krhino_sem_give(&group->res_sem);

    if (reply.magic == RES_MSG_MAGIC) {
        return (void*)reply.ret_val;
    }

    return NULL;
}

void u_res_free(int pid, void *ptr)
{
    task_group_t *group;
    res_request_msg_t request;
    size_t reply_size;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
        return;
    }

    memset(&request, 0, sizeof(res_request_msg_t));
    request.magic = RES_MSG_MAGIC;
    request.type = REQUEST_FREE;
    request.arg_cnt = 1;
    request.has_ret = 0;
    request.arg[0] = (unsigned int)ptr;

    krhino_sem_take(&group->res_sem, RHINO_WAIT_FOREVER);
    krhino_buf_queue_send(group->res_request_q, &request, sizeof(res_request_msg_t));
    krhino_sem_give(&group->res_sem);
}

void u_res_kill(int pid)
{
    task_group_t *group;
    res_request_msg_t request;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
    }

    memset(&request, 0, sizeof(res_request_msg_t));
    request.magic = RES_MSG_MAGIC;
    request.type = REQUEST_EXIT;
    request.arg_cnt = 0;
    request.has_ret = 0;

    krhino_mutex_lock(&res_mutex, RHINO_WAIT_FOREVER);
    krhino_buf_queue_send(group->res_request_q, &request, sizeof(res_request_msg_t));
    krhino_mutex_unlock(&res_mutex);
}

