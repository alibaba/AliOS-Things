/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "cli/cli_api.h"
#include "utask.h"
#include "uassert.h"

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

int ucli_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio)
{
    kstat_t stat;

    stat = krhino_utask_dyn_create(&ucli_task_obj, "ucli_task", NULL, prio, (tick_t)50,
                                   ustack_size, kstack_size, ucli_task, 1);

    if (stat != RHINO_SUCCESS) {
        uassert(0);
    }

    return 0;
}

