/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <k_api.h>
#include <u_task.h>
#include <cli_api.h>

#define UCLI_TASK_USTACK 512u
#define UCLI_TASK_KSTACK 512u

typedef void (*cli_func)(void);

ktask_t *ucli_task_obj;

void ucli_task(void *arg)
{
    void                  *res;
    struct cli_command_st *cmd;

    krhino_uprocess_res_get(2, &res);
    if (res == NULL) {
        printf("%s: get cli queue failed\r\n", __func__);
        while(1) {
            krhino_task_sleep(10000);
        }
    }

    while (1) {
        krhino_queue_recv(res, RHINO_WAIT_FOREVER, (void**)&cmd);
        if (cmd != 0) {
            if (cmd->function)
                ((cli_func)cmd->function)();
        }
    }
}

int ucli_task_start(void)
{
    kstat_t      stat;
    cpu_stack_t *ustack_buf;

    ustack_buf = (cpu_stack_t*)malloc(UCLI_TASK_USTACK * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        return -1;
    }

    stat = krhino_utask_create(&ucli_task_obj, "ucli_task", 0, 25 ,50 , ustack_buf,
                               UCLI_TASK_USTACK, UCLI_TASK_KSTACK, ucli_task, 1);

    if (stat != RHINO_SUCCESS) {
        free(ustack_buf);
        return -2;
    }

    return 0;
}

