/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "u_api.h"
#include "cli_api.h"

#define UAPP_USTACK_SIZE 0x100
#define UAPP_KSTACK_SIZE 0x200

#define MSG_KEY1 0x1234
#define MSG_KEY2 0x4321

#if (RHINO_CONFIG_UCLI_TASK > 0)

int g_proc_var;

static void uapp2_read_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    printf("uapp2 read g_proc_var: %d\r\n", g_proc_var);
}

static void uapp2_set_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: uapp2_set_g_proc_var val\r\n");
        return;
    }

    g_proc_var = atoi(argv[1]);

    printf("uapp2 set g_proc_var: %d\r\n", g_proc_var);
}

static void uapp2_proc_msg_test_task_main(void)
{
    size_t recv_queue, send_queue;
    size_t msg_sz;
    char   msg[32];
    int    send_var, recv_var;
    int    i;
    int    reason;
    int    ret;

    recv_queue = krhino_msg_get(MSG_KEY1, 0, 100);
    if(!recv_queue) {
        printf("uapp2 create ipc recv_queue failed\r\n");
        return;
    }

    send_queue = krhino_msg_get(MSG_KEY2, 0, 100);
    if(!send_queue) {
        printf("uapp2 create ipc send_queue failed\r\n");
        return;
    }

    for (i = 0; i < 1000; i++, send_var++) {
        memset(msg, 0, sizeof(msg));
        ret = krhino_msg_recv(recv_queue, 1000, msg, &msg_sz);
        if (ret == RHINO_SUCCESS) {
            recv_var = atoi(msg);
            recv_var++;
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "%d", recv_var);
            krhino_msg_snd(send_queue, msg, strlen(msg));
        } else {
            printf("uapp2 recev error ");
            if (ret == RHINO_BLK_TIMEOUT) {
                printf("reason: timeout\r\n");
            } else {
                printf("krhino_msg_recv 0x%x\r\n", ret);
            }
            break;
        }
    }

    if (i != 1000) {
        printf("uapp2 proc_msg_test failed ");
    } else {
        printf("uapp2 proc_msg_test success\r\n");
    }

    krhino_msg_del(recv_queue);
    krhino_msg_del(send_queue);

    krhino_utask_del(NULL);
}

static void uapp2_proc_msg_test(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;

    krhino_utask_dyn_create(&task, "tcp_download_task", NULL, 20, 5,
                            UAPP_USTACK_SIZE, UAPP_KSTACK_SIZE,
                            uapp2_proc_msg_test_task_main, 1);
}

static struct cli_command_st ucmds[] = {
    {"uapp2_read_g_proc_var", "uapp2 read g_proc_var", uapp2_read_g_proc_var},
    {"uapp2_set_g_proc_var", "uapp2 set g_proc_var", uapp2_set_g_proc_var},
    {"uapp2_proc_msg_test", "uapp2 proc_msg_test", uapp2_proc_msg_test},
};

#endif

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    printf("uapp2 start to run...\r\n");

#if (RHINO_CONFIG_UCLI_TASK > 0)
    cli_register_commands(ucmds, sizeof(ucmds)/sizeof(ucmds[0]));
#endif

    /* don't return in the first task, only if the app is to exit */
    while (1) {
        krhino_task_sleep(200);
    }
}

