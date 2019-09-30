/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "u_api.h"

#define UAPP_USTACK_SIZE 0x100
#define UAPP_KSTACK_SIZE 0x200

#define MSG_KEY1 0x1234
#define MSG_KEY2 0x4321

#if (RHINO_CONFIG_UCLI_TASK > 0)
#include "cli/cli_api.h"

int g_proc_var;

static void uapp1_exit(char *pbuffer, int outlen, int argc, char **argv)
{
    krhino_uprocess_exit();
}

extern test_certificate(void);
static void certificate_task_main(void *arg)
{
    test_certificate();

    krhino_utask_del(NULL);
}

static void certificate_test(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;

    krhino_utask_dyn_create(&task, "certificate_task", NULL, 20, 5,
                            UAPP_USTACK_SIZE, UAPP_KSTACK_SIZE,
                            certificate_task_main, 1);
}

#if (RHINO_CONFIG_UTCPIP > 0)
extern int net_parse_host(void);
static void net_parse_host_task_main(void *arg)
{
    int ret;

    ret = net_parse_host();
    if (!ret) {
        printf("%s test success\r\n", __func__);
    } else {
        printf("%s test failed, ret %d\r\n", __func__, ret);
    }

    krhino_utask_del(NULL);
}

static void net_parse_host_test(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;

    krhino_utask_dyn_create(&task, "net_parse_host_task", NULL, 20, 5,
                            UAPP_USTACK_SIZE, UAPP_KSTACK_SIZE,
                            net_parse_host_task_main, 1);
}

extern int tcp_download_file(void);
static void tcp_download_task_main(void *arg)
{
    int ret;

    ret = tcp_download_file();

    if (ret == 0) {
        printf("%s test success\r\n", __func__);
    } else {
        printf("%s test failed, ret %d\r\n", __func__, ret);
    }

    krhino_utask_del(NULL);
}

static void tcp_download_test(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;

    krhino_utask_dyn_create(&task, "tcp_download_task", NULL, 20, 5,
                            UAPP_USTACK_SIZE, UAPP_KSTACK_SIZE,
                            tcp_download_task_main, 1);
}
#endif

static void uapp1_read_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    printf("uapp1 read g_proc_var: %d\r\n", g_proc_var);
}

static void uapp1_set_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: uapp1_set_g_proc_var val\r\n");
        return;
    }

    g_proc_var = atoi(argv[1]);

    printf("uapp1 set g_proc_var: %d\r\n", g_proc_var);
}

static void uapp1_proc_msg_test_task_main(void)
{
    size_t send_queue, recv_queue;
    size_t msg_sz;
    char   msg[32];
    int    send_var, recv_var;
    int    i;
    int    reason;
    int    ret;

    send_queue = krhino_msg_get(MSG_KEY1, 0, 100);
    if(!send_queue) {
        printf("uapp1 create ipc send_queue failed\r\n");
        return;
    }

    recv_queue = krhino_msg_get(MSG_KEY2, 0, 100);
    if(!recv_queue) {
        printf("uapp1 create ipc recv_queue failed\r\n");
        return;
    }

    send_var = 0;
    for (i = 0; i < 1000; i++, send_var++) {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "%d", send_var);
        krhino_msg_snd(send_queue, msg, strlen(msg));

        memset(msg, 0, sizeof(msg));
        ret = krhino_msg_recv(recv_queue, 1000, msg, &msg_sz);
        if (ret == RHINO_SUCCESS) {
            recv_var = atoi(msg);
            if (recv_var != send_var + 1) {
                printf("error: uapp1 send_var %d, recv_var %d\r\n", send_var, recv_var);
                break;
            }
        } else {
            printf("uapp1 recev error ,ret %d\r\n", ret);
        }
    }

    if (i != 1000) {
        printf("uapp1 proc_msg_test failed ");
        if (ret == RHINO_BLK_TIMEOUT) {
            printf("reason: recv timeout\r\n");
        } else {
            printf("krhino_msg_recv ret 0x%x\r\n", ret);
        }
    } else {
        printf("uapp1 proc_msg_test success\r\n");
    }

    krhino_msg_del(send_queue);
    krhino_msg_del(recv_queue);

    krhino_utask_del(NULL);
}

static void uapp1_proc_msg_test(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;

    krhino_utask_dyn_create(&task, "tcp_download_task", NULL, 20, 5,
                            UAPP_USTACK_SIZE, UAPP_KSTACK_SIZE,
                            uapp1_proc_msg_test_task_main, 1);
}

static struct cli_command_st ucmds[] = {
    {"uapp1_exit", "uapp1 process exit", uapp1_exit},
    {"certificate", "run user space certificate test", certificate_test},
#if (RHINO_CONFIG_UTCPIP > 0)
    {"net_parse_host", "parse net host", net_parse_host_test},
    {"tcp_download", "user space tcp download test", tcp_download_test},
#endif
    {"uapp1_read_g_proc_var", "uapp1 read g_proc_var", uapp1_read_g_proc_var},
    {"uapp1_set_g_proc_var", "uapp1 set g_proc_var", uapp1_set_g_proc_var},
    {"uapp1_proc_msg_test", "uapp1 proc_msg_test", uapp1_proc_msg_test},
};

#endif

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    printf("uapp1 start to run...\r\n");

#if (RHINO_CONFIG_UCLI_TASK > 0)
    cli_register_commands(ucmds, sizeof(ucmds)/sizeof(ucmds[0]));
#endif

    /* don't return in the first task, only if the app is to exit */
    while (1) {
        krhino_task_sleep(200);
    }
}

