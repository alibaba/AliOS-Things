/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <u_task.h>
#include <mm.h>
#include <cli_api.h>
#include <errno.h>

#define UCLI_TASK_USTACK 512u
#define UCLI_TASK_KSTACK 512u

ktask_t *ucli_task_obj;

void ucli_task(void *arg)
{
    void *res;
    void *msg;

    krhino_uprocess_res_get(2, &res);

    while (1) {
        krhino_queue_recv(res, RHINO_WAIT_FOREVER, &msg);
        if (msg != 0) {
            printf("user cli cmd : %s\n", ((struct cli_command_st *)msg)->name);
        }
    }
}

kstat_t ucli_task_start(void)
{
    kstat_t ret;

    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(UCLI_TASK_USTACK * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        return -ENOMEM;
    }

    ret = krhino_utask_create(&ucli_task_obj, "ucli_task", 0, 25 ,50 , ustack_buf,
                               UCLI_TASK_USTACK, UCLI_TASK_KSTACK, ucli_task, 1);

    if (ret != RHINO_SUCCESS) {
        free(ustack_buf);
        return ret;
    }

    return ret;
}

