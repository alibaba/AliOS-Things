/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <u_task.h>
#include <u_callback.h>
#include <aos/log.h>
#include <stdio.h>

#define CB_TASK_USTACK 0x4000 // 16k
#define RESV_CB_TASK_USTACK 0x4000 // 16k

#define CB_TASK_KSTACK 0x4000 // 16k

typedef int (*f0)(void);
typedef int (*f1)(unsigned int arg1);
typedef int (*f2)(unsigned int arg1, unsigned int arg2);
typedef int (*f3)(unsigned int arg1, unsigned int arg2, unsigned int arg3);
typedef int (*f4)(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);
typedef int (*f5)(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5);

ktask_t    *cb_task_obj;
static cpu_stack_t cb_task_ustack[CB_TASK_USTACK];

static int callback_handler(void *fptr, uint16_t arg_cnt, uint32_t *value)
{
    int ret;

    if (fptr == NULL) {
        LOG("%s: fptr NULL\r\n", __func__);
        return 0;
    }

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
        default:
            LOG("%s: error arg_cnt %d\r\n", __func__, arg_cnt);
            break;
    }

    return ret;
}

void cb_task(void *arg)
{
    kbuf_queue_t *call_buf_queue;
    kbuf_queue_t *ret_buf_queue;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    kstat_t stat;
    size_t size;
    int msg_ret_value;

    krhino_uprocess_res_get(USER_CB_CALL_BUF_QUEUE, (void**)&call_buf_queue);
    if (NULL == call_buf_queue) {
        LOG("%s: get resource failed\r\n", __func__);
        return;
    }

    krhino_uprocess_res_get(USER_CB_RET_BUF_QUEUE, (void**)&ret_buf_queue);
    if (NULL == ret_buf_queue) {
        LOG("%s: get resource failed\r\n", __func__);
        return;
    }

    while (1) {
        stat = krhino_buf_queue_recv(call_buf_queue, RHINO_WAIT_FOREVER, &call_msg, &size);
        if (RHINO_SUCCESS != stat) {
            LOG("%s: recv msg error\r\n", __func__);
            break;
        }

        if (size != sizeof(cb_call_msg_t)) {
            LOG("%s: recv msg size 0x%x error, expected 0x%x\r\n",
                   __func__, size, sizeof(cb_call_msg_t));
            break;
        }

        if (call_msg.magic != MSG_CALL_MAGIC) {
            LOG("%s: call_msg.magic 0x%x error, expected 0x%x\r\n",
                    __func__, call_msg.magic, MSG_CALL_MAGIC);
            break;
        }

        msg_ret_value = callback_handler(call_msg.func_ptr,
                                         call_msg.arg_cnt,
                                         call_msg.arg_value);

        if (call_msg.has_ret) {
            ret_msg.magic = MSG_RET_MAGIC;
            ret_msg.ret_val = msg_ret_value;

            LOG("cb send reply, value 0x%x\r\n", ret_msg.ret_val);
            stat = krhino_buf_queue_send(ret_buf_queue, &ret_msg, sizeof(cb_ret_msg_t));
            if (RHINO_SUCCESS != stat) {
                LOG("%s: cb send failed, ret 0x%x\r\n", __func__, stat);
            }
        }
    }
}

void cb_task_start(void)
{
    kstat_t stat;

    stat = krhino_utask_create(&cb_task_obj, "callback_task",
                               0, 20 ,50 ,cb_task_ustack,
                               CB_TASK_USTACK, CB_TASK_KSTACK,
                               cb_task, 1);
    if (RHINO_SUCCESS != stat) {
        printf("%s: create callback_task failed, ret 0x%x\r\n",
               __func__, stat);
        return;
    }
}

