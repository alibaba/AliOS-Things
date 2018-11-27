/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <u_task.h>

#define RES_TASK_USTACK 512u
#define RES_TASK_KSTACK 512u

ktask_t    *res_task_obj;
cpu_stack_t ustack_buf[RES_TASK_USTACK];

void res_task(void *arg)
{
    void *res;
    void *msg;

    krhino_uprocess_res_get(0, &res);
    while (1) {
        krhino_queue_recv(res, RHINO_WAIT_FOREVER, &msg);
        free(msg);
        krhino_uprocess_res_get(1, &res);
        if (res != 0) {
            krhino_utask_del(res_task_obj);
        }
    }
}

void res_task_start(void)
{
    krhino_utask_create(&res_task_obj, "res_task", 0, 20 ,50 ,ustack_buf,
                        RES_TASK_USTACK, RES_TASK_KSTACK, res_task, 1);
}

