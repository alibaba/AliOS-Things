/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <k_api.h>
#include <parcel.h>
#include <rpc.h>
#include <aos/signal.h>
#include <procmgmt.h>
#include "process_service.h"

#include "aos/cli.h"

static void load_proc_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;

    if (argc != 2) {
        aos_cli_printf("Usage: load app_name\r\n");
        return;
    }

    ret = aos_process_load(argv[1]);

    if (ret > 0) {
        aos_cli_printf("load %s success\r\n", argv[1]);
    } else {
        aos_cli_printf("load %s failed\r\n", argv[1]);
    }
}

static void kill_proc_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;
    uint32_t pid;
    char name_buf[PROC_NAME_MAX];
    int sig = 0;

    if (argc == 2) {
        pid = atoi(argv[1]);

        memset(name_buf, 0, PROC_NAME_MAX);
        proc_map_name_get(pid, name_buf);
        ret = aos_process_unload(name_buf);

        if (!ret) {
            aos_cli_printf("unload %s success\r\n", argv[1]);
        } else {
            aos_cli_printf("unload %s failed\r\n", argv[1]);
        }
        return;
    } else if (argc ==3) {
        sig = atoi(argv[1]+1);
        pid = atoi(argv[2]);
        aos_kill(pid, sig);
    } else {
        aos_cli_printf("Usage: kill [-signum] pid\r\n");
        return;
    }
}

static void ps_proc_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    proc_map_show();
}

static struct cli_command ucmds[] = {
    {"load", "load process", load_proc_cmd},
    {"kill", "kill process", kill_proc_cmd},
    {"ps", "show process info", ps_proc_cmd},
};

void serive_proc_cmd_init(void)
{
    aos_cli_register_commands(ucmds, sizeof(ucmds)/sizeof(ucmds[0]));
}

