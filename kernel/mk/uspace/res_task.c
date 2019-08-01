/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "utask.h"
#include "res.h"
#include "uassert.h"

ktask_t *res_task_obj;

void res_task(void *arg)
{
    void              *res_request_q;
    void              *res_reply_q;
    res_request_msg_t  request_msg;
    res_reply_msg_t    reply_msg;
    void              *user_ptr;
    size_t             buf_size;
    size_t             size;

    res_request_q = NULL;
    res_reply_q   = NULL;

    krhino_uprocess_res_get(USER_RES_REQUEST_BUF_QUEUE, &res_request_q);
    uassert(res_request_q != NULL);

    krhino_uprocess_res_get(USER_RES_REPLY_BUF_QUEUE, &res_reply_q);
    uassert(res_reply_q != NULL);

    while (1) {
        krhino_buf_queue_recv(res_request_q, RHINO_WAIT_FOREVER, &request_msg, &size);
        switch(request_msg.type) {
            case REQUEST_MALLOC:
                buf_size = (size_t)request_msg.arg[0];
                user_ptr = (void*)malloc(buf_size);
                uassert (NULL != user_ptr);

                reply_msg.ret_val = user_ptr;
                krhino_buf_queue_send(res_reply_q, &reply_msg, sizeof(res_reply_msg_t));
                break;
            case REQUEST_FREE:
                user_ptr = request_msg.arg[0];
                free(user_ptr);
                break;
            default:
                uassert(0);
                break;
        }
    }
}

int res_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio)
{
    kstat_t stat;

    stat = krhino_utask_dyn_create(&res_task_obj, "res_task", NULL, prio, (tick_t)50,
                                   ustack_size, kstack_size, res_task, 1u);

    uassert(stat == RHINO_SUCCESS);

    return 0;
}

