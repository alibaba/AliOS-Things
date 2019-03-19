/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <k_api.h>
#include <u_task.h>
#include <u_res.h>
#include <stdio.h>

#define RES_TASK_USTACK 0x4000 // 16k
#define RES_TASK_KSTACK 0x4000 // 16k

ktask_t    *res_task_obj;
cpu_stack_t ustack_buf[RES_TASK_USTACK];

void res_task(void *arg)
{
    kstat_t            stat;
    void              *res_request_q;
    void              *res_reply_q;
    res_request_msg_t  request_msg;
    res_reply_msg_t    reply_msg;
    void              *user_ptr;
    unsigned int       buf_size;
    size_t             size;

    res_request_q = NULL;
    krhino_uprocess_res_get(USER_RES_REQUEST_BUF_QUEUE, &res_request_q);
    if (NULL == res_request_q) {
        printf("%s: get res_request_q failed\r\n", __func__);
        goto err;
    }

    res_reply_q = NULL;
    krhino_uprocess_res_get(USER_RES_REPLY_BUF_QUEUE, &res_reply_q);
    if (NULL == res_reply_q) {
        printf("%s: get res_reply_q failed\r\n", __func__);
        goto err;
    }

    while (1) {
        stat = krhino_buf_queue_recv(res_request_q, RHINO_WAIT_FOREVER,
                                     (void*)&request_msg, &size);
        if (stat != RHINO_SUCCESS) {
            printf("%s: error, queue recv failed, ret 0x%x\r\n", __func__, stat);
            goto err;
        }

        if (size != sizeof(res_request_msg_t)) {
            printf("%s: error, recv size 0x%x error\r\n", __func__, size);
            goto err;
        }

        if (request_msg.magic != RES_MSG_MAGIC) {
            printf("%s: error, magic number 0x%x\r\n", __func__, request_msg.magic);
            goto err;
        }

        switch(request_msg.type) {
            case REQUEST_MALLOC:
                buf_size = request_msg.arg[0];
                printf("%s: malloc request, size 0x%x\r\n", __func__, buf_size);
                user_ptr = malloc(buf_size);
                reply_msg.magic = RES_MSG_MAGIC;
                reply_msg.ret_val = (unsigned int)user_ptr;
                stat = krhino_buf_queue_send(res_reply_q, &reply_msg, sizeof(res_reply_msg_t));
                if (stat != RHINO_SUCCESS) {
                    printf("%s: send reply_msg failed, ret 0x%x\r\n", __func__, stat);
                    goto err;
                }
                break;
            case REQUEST_FREE:
                user_ptr = (void*)request_msg.arg[0];
                printf("%s: free request, ptr 0x%x\r\n", __func__, user_ptr);
                free(user_ptr);
                break;
            case REQUEST_EXIT:
		printf("%s: process try exit request\r\n", __func__);
		//krhino_uprocess_exit();
                krhino_uprocess_try_exit();
                break;
            default:
                printf("%s: unsupported reqest type 0x%x\r\n", __func__, request_msg.type);

        }
    }
err:
    printf("%s: suspend res_task\r\n", __func__);
    krhino_task_suspend(krhino_cur_task_get());
}

int res_task_start(void)
{
    kstat_t stat;

    stat = krhino_utask_create(&res_task_obj, "res_task", 0, 20 ,50 ,ustack_buf,
                        RES_TASK_USTACK, RES_TASK_KSTACK, res_task, 1);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    return 0;
}

