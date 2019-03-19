/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <k_api.h>
#include <u_task.h>
#include <aos/cli.h>
#include <errno.h>

#define UCLI_TASK_USTACK 512u
#define UCLI_TASK_KSTACK 512u

typedef void (*cli_func)(char *pbuffer, int outlen, int argc, char **argv);

ktask_t *ucli_task_obj;

void ucli_task(void *arg)
{
    kstat_t stat;
    void *res;
    ucli_msg_t ucli_msg;
    size_t size;

    krhino_uprocess_res_get(USER_CLI_BUF_QUEUE, &res);
    if (res == NULL) {
        printf("%s: get cli queue failed\r\n", __func__);
        krhino_utask_del(NULL);
    }

    while (1) {
        stat = krhino_buf_queue_recv(res, RHINO_WAIT_FOREVER, (void*)&ucli_msg, &size);
        if (RHINO_SUCCESS == stat) {
            if (ucli_msg.magic != UCLI_MSG_MAGIC) {
                printf("%s: ucli_msg magic error, value 0x%x\r\n", __func__, ucli_msg.magic);
                continue;
            }

            if (ucli_msg.func)
                ((cli_func)ucli_msg.func)(NULL, 0, ucli_msg.argc, ucli_msg.argv);

            if (ucli_msg.argc > 0) {
                free((void*)ucli_msg.argv);
            }
        }
    }
}

int ucli_task_start(void)
{
    kstat_t stat;

    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(UCLI_TASK_USTACK * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        printf("%s: alloc ucli task stack failed\r\n", __func__);
        return -1;
    }

    stat = krhino_utask_create(&ucli_task_obj, "ucli_task", 0, 25 ,50 , ustack_buf,
                               UCLI_TASK_USTACK, UCLI_TASK_KSTACK, ucli_task, 1);

    if (stat != RHINO_SUCCESS) {
        printf("%s: create ucli task failed\r\n", __func__);
        free(ustack_buf);
        return -2;
    }

    return 0;
}

