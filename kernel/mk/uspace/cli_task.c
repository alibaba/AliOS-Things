/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "u_api.h"

#if (RHINO_CONFIG_UCLI_SUPPORT > 0)

typedef void (*cli_func)(char *pbuffer, int outlen, int argc, char **argv);

ktask_t *ucli_task_obj;

void ucli_task(void *arg)
{
    kstat_t     stat;
    void       *res;
    ucli_msg_t  ucli_msg;
    size_t      size;

    krhino_uprocess_res_get(USER_CLI_BUF_QUEUE, &res);
    uassert(NULL != res);

    while (1) {
        stat = krhino_buf_queue_recv(res, RHINO_WAIT_FOREVER, &ucli_msg, &size);
        if (RHINO_SUCCESS == stat) {
            if (NULL != ucli_msg.func) {
                ((cli_func)ucli_msg.func)(NULL, 0, ucli_msg.argc, ucli_msg.argv);
            }

            if (ucli_msg.argc > 0) {
                free(ucli_msg.argv);
            }
        }
    }
}

int ucli_task_start(void)
{
    kstat_t      stat;
    cpu_stack_t *ustack_buf;

    ustack_buf = (cpu_stack_t*)malloc(RHINO_CONFIG_UCLI_TASK_USTACK_SIZE * sizeof(cpu_stack_t));
    uassert(NULL != ustack_buf);

    stat = krhino_utask_create(&ucli_task_obj, "ucli_task", 0,
                               RHINO_CONFIG_UCLI_TASK_PRIO ,50 , ustack_buf,
                               RHINO_CONFIG_UCLI_TASK_USTACK_SIZE,
                               RHINO_CONFIG_UCLI_TASK_KSTACK_SIZE, ucli_task, 1);

    if (stat != RHINO_SUCCESS) {
        free(ustack_buf);
        uassert(0);
    }

    return 0;
}

#endif /* RHINO_CONFIG_UCLI_SUPPORT */

