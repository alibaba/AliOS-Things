/**
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "u_api.h"

#if (RHINO_CONFIG_UCALLBACK_SUPPORT > 0)

typedef int (*f0)(void);
typedef int (*f1)(void *arg1);
typedef int (*f2)(void *arg1, void *arg2);
typedef int (*f3)(void *arg1, void *arg2, void *arg3);
typedef int (*f4)(void *arg1, void *arg2, void *arg3, void *arg4);
typedef int (*f5)(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
typedef int (*f6)(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6);
typedef int (*f7)(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6, void *arg7);
typedef int (*f8)(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6, void *arg7, void *arg8);

static ktask_t    *cb_task_obj;
static cpu_stack_t cb_task_ustack[RHINO_CONFIG_UCALLBACK_TASK_USTACK_SIZE];

static int callback_handler(void *fptr, uint16_t arg_cnt, void *value[])
{
    int ret;

    uassert(NULL != fptr);

    switch(arg_cnt) {
        case 0:
            ret = ((f0)fptr)();
            break;
        case 1:
            ret = ((f1)fptr)(value[0]);
            break;
        case 2:
            ret = ((f2)fptr)(value[0], value[1]);
            break;
        case 3:
            ret = ((f3)fptr)(value[0], value[1], value[2]);
            break;
        case 4:
            ret = ((f4)fptr)(value[0], value[1], value[2], value[3]);
            break;
        case 5:
            ret = ((f5)fptr)(value[0], value[1], value[2], value[3], value[4]);
            break;
        case 6:
            ret = ((f6)fptr)(value[0], value[1], value[2], value[3], value[4], value[5]);
            break;
        case 7:
            ret = ((f7)fptr)(value[0], value[1], value[2], value[3], value[4], value[5], value[6]);
            break;
        case 8:
            ret = ((f8)fptr)(value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7]);
            break;
        default:
            uassert(0);
            break;
    }

    return ret;
}

void cb_task(void *arg)
{
    kbuf_queue_t  *call_buf_queue;
    kbuf_queue_t  *ret_buf_queue;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    kstat_t       stat;
    size_t        size;
    int           msg_ret_value;

    krhino_uprocess_res_get(USER_CB_CALL_BUF_QUEUE, (void**)&call_buf_queue);
    uassert(NULL != call_buf_queue);

    krhino_uprocess_res_get(USER_CB_RET_BUF_QUEUE, (void**)&ret_buf_queue);
    uassert(NULL != ret_buf_queue);

    while (1) {
        krhino_buf_queue_recv(call_buf_queue, RHINO_WAIT_FOREVER, &call_msg, &size);

        msg_ret_value = callback_handler(call_msg.func_ptr,
                                         call_msg.arg_cnt,
                                         call_msg.arg_value);

        if (call_msg.has_ret) {
            ret_msg.ret_val = (void*)msg_ret_value;

            stat = krhino_buf_queue_send(ret_buf_queue, &ret_msg, sizeof(cb_ret_msg_t));
            uassert(RHINO_SUCCESS == stat);
        }
    }
}

void cb_task_start(void)
{
    kstat_t stat;

    stat = krhino_utask_create(&cb_task_obj, "callback_task", 0,
                               RHINO_CONFIG_UCALLBACK_TASK_PRIO,
                               50 ,cb_task_ustack,
                               RHINO_CONFIG_UCALLBACK_TASK_USTACK_SIZE,
                               RHINO_CONFIG_UCALLBACK_TASK_KSTACK_SIZE,
                               cb_task, 1);

    uassert(RHINO_SUCCESS == stat);
}

#endif /* RHINO_CONFIG_UCALLBACK_SUPPORT */

