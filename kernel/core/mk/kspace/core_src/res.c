/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "aos/kernel.h"
#include <stdio.h>
#include <fcntl.h>
#include "k_api.h"
#include "res.h"
#include "utask.h"

static void *buf_queue_send_recv(int pid, res_request_msg_t *request)
{
    kstat_t stat;
    task_group_t *group;
    res_reply_msg_t reply;
    size_t reply_size;
    aos_hdl_t send_hdl, rep_hdl;

    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return NULL;
    }

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_RES_REQUEST_BUF_QUEUE, pid);
    stat = aos_queue_open(&send_hdl, name1, O_CREAT, (sizeof(res_request_msg_t) + sizeof(intptr_t)) * RES_QUEUE_MSG_NUM,
                          sizeof(res_request_msg_t));
    if (stat != RHINO_SUCCESS) {
        printf("####### aos_queue_open1 ret %d\r\n", stat);
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    snprintf(name2, USER_NAME_MAX, "%s%d", USER_RES_REPLY_BUF_QUEUE, pid);
    stat = aos_queue_open(&rep_hdl, name2, O_CREAT, (sizeof(res_reply_msg_t) + sizeof(intptr_t)) * RES_QUEUE_MSG_NUM,
                          sizeof(res_reply_msg_t));
    if (stat != RHINO_SUCCESS) {
        printf("####### aos_queue_open2 ret %d\r\n", stat);
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    stat = krhino_mutex_lock(&group->res_mutex, RHINO_WAIT_FOREVER);

    stat = aos_queue_send(&send_hdl, request, sizeof(*request));
    if (stat != RHINO_SUCCESS) {
        printf("####### krhino_buf_queue_send ret %d\r\n", stat);
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    memset(&reply, 0, sizeof(res_reply_msg_t));
    stat = aos_queue_recv(&rep_hdl, AOS_WAIT_FOREVER, &reply, &reply_size);
    if (stat != RHINO_SUCCESS) {
        printf("####### krhino_buf_queue_send ret %d\r\n", stat);
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    krhino_mutex_unlock(&group->res_mutex);
    aos_queue_close(&send_hdl);
    aos_queue_close(&rep_hdl);
    return reply.ret_val;
}

void *res_malloc(int pid, size_t len)
{
    res_request_msg_t request;
    void *ptr;

    memset(&request, 0, sizeof(res_request_msg_t));
    request.type = REQUEST_MALLOC;
    request.arg_cnt = 1;
    request.arg[0] = (void *)len;

    ptr = buf_queue_send_recv(pid, &request);

    return ptr;
}

void res_free(int pid, void *ptr)
{
    res_request_msg_t request;

    memset(&request, 0, sizeof(res_request_msg_t));
    request.type = REQUEST_FREE;
    request.arg_cnt = 1;
    request.arg[0] = ptr;

    buf_queue_send_recv(pid, &request);
}

int res_initialize(task_group_t *group)
{
    kstat_t stat;
    aos_hdl_t res_req_queue, res_reply_queue;
    uint32_t pid = group->pid;
    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};
    snprintf(name1, USER_NAME_MAX, "%s%d", USER_RES_REQUEST_BUF_QUEUE, pid);
    stat = aos_queue_open(&res_req_queue, name1, O_CREAT,
                          (sizeof(res_request_msg_t) + sizeof(intptr_t)) * RES_QUEUE_MSG_NUM, sizeof(res_request_msg_t));

    if (stat != 0) {
        return -1;
    }

    snprintf(name2, USER_NAME_MAX, "%s%d", USER_RES_REPLY_BUF_QUEUE, pid);
    stat = aos_queue_open(&res_reply_queue, name2, O_CREAT,
                          (sizeof(res_reply_msg_t) + sizeof(intptr_t)) * RES_QUEUE_MSG_NUM, sizeof(res_reply_msg_t));

    if (stat != 0) {
        aos_queue_close(&res_req_queue);
        aos_queue_unlink(name1);
        return -2;
    }

    aos_queue_close(&res_req_queue);
    aos_queue_close(&res_reply_queue);
    krhino_mutex_create(&group->res_mutex, "res_mutex");

    return 0;
}

void res_exit(task_group_t *group)
{
    int pid = group->pid;
    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_RES_REQUEST_BUF_QUEUE, pid);
    snprintf(name2, USER_NAME_MAX, "%s%d", USER_RES_REPLY_BUF_QUEUE, pid);

    aos_queue_unlink(name1);
    aos_queue_unlink(name2);

    krhino_mutex_del(&group->res_mutex);
}
