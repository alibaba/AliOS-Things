/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <u_task.h>

#define RES_TASK_USTACK  512u
#define RES_TASK_KSTACK  512u

ktask_t *res_task;

void res_task(void *arg)
{
    void *res;
    void *msg;

    krhino_uprocess_res_get(0, &res);
    while (1) {
        krhino_queue_recv(res, RHINO_WAIT_FOREVER, &msg);
        free(msg);
    }
}

void res_task_start(void)
{
    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(RES_TASK_USTACK * sizeof(cpu_stack_t));
    krhino_utask_create(&res_task, "res_task", 0, 20 ,50 ,ustack_buf, RES_TASK_USTACK, 
                         RES_TASK_KSTACK, res_task, 1);
}

