/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "irq/irqdesc.h"
#include "u_api.h"
#include "ulog/ulog.h"

#include <procmgmt.h>

#define UAPP_USTACK_SIZE 0x1000
#define UAPP_KSTACK_SIZE 0x1000

#if (RHINO_CONFIG_UCLI_TASK > 0)
#include "aos/cli.h"

static void uapp1_exit(char *pbuffer, int outlen, int argc, char **argv)
{
    krhino_uprocess_exit();
}

static void help_cmd(char *buf, int len, int argc, char **argv)
{
    printf("==== %s CLI Cmd ====\r\n", RHINO_CONFIG_APP_NAME);
    cli_cmds_show();
}

static struct cli_command ucmds[] = {
    {"help", "print this", help_cmd},
    {"uapp1_exit", "uapp1 process exit", uapp1_exit},
};

#endif
void testirq()
{
    printf("app testirq\r\n");
}

int application_start(int argc, char *argv[])
{
    int pid = 0;
#if (RHINO_CONFIG_UCLI_TASK > 0)
    aos_cli_register_commands(ucmds, sizeof(ucmds)/sizeof(ucmds[0]));
#endif

    /* try to load uapp_helloworld2*/
    pid = aos_process_load("/system/uapp_helloworld2@vexpressa9-mk.app.stripped.elf");
    krhino_task_sleep(5000);
    if (pid > 0) {
        aos_process_unload("/system/uapp_helloworld2@vexpressa9-mk.app.stripped.elf");
    }


    while (1) {
        printf("hello world! I am APP1 in userspace\r\n");
        krhino_task_sleep(5000);
    }
}

