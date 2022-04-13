/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "u_api.h"
#include "ulog/ulog.h"

extern int genie_stars_entry(void);

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

int application_start(int argc, char *argv[])
{
    while (1) {
        printf("hello world! I am APP2 in userspace\r\n");
        krhino_task_sleep(1000);
    }
}

