/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "utask.h"
#include "res.h"
#include "uassert.h"
#include "aos/kernel.h"

ktask_t *res_task_obj;

void res_task(void *arg)
{
    int stat;
    res_request_msg_t  request_msg;
    res_reply_msg_t    reply_msg;
    void              *user_ptr;
    size_t             buf_size;
    size_t             size;

    aos_hdl_t res_request_q, res_reply_q;
    uint32_t pid = aos_getpid();
    char name1[USER_NAME_MAX + 1] = {0};
    char name2[USER_NAME_MAX + 1] = {0};

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_RES_REQUEST_BUF_QUEUE, pid);

    stat = aos_queue_open(&res_request_q, name1, 0, 0, 0);
    uassert(stat == 0);

    snprintf(name2, USER_NAME_MAX, "%s%d", USER_RES_REPLY_BUF_QUEUE, pid);
    stat = aos_queue_open(&res_reply_q, name2, 0, 0, 0);
    uassert(stat == 0);

    while (1) {
        stat = aos_queue_recv(&res_request_q, AOS_WAIT_FOREVER, &request_msg, &size);
        uassert(stat == 0);

        switch (request_msg.type) {
            case REQUEST_MALLOC:
                buf_size = (size_t)request_msg.arg[0];
                user_ptr = (void *)malloc(buf_size);
                uassert(NULL != user_ptr);

                reply_msg.ret_val = user_ptr;
                aos_queue_send(&res_reply_q, &reply_msg, sizeof(res_reply_msg_t));
                break;
            case REQUEST_FREE:
                user_ptr = request_msg.arg[0];
                free(user_ptr);
                reply_msg.ret_val = 0;
                aos_queue_send(&res_reply_q, &reply_msg, sizeof(res_reply_msg_t));
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

