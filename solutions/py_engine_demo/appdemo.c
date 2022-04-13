/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "mpy_main.h"
#include "aos/cli.h"




static void handle_identity_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    //printf("argc = %d;argv = %p; %s;\n", argc, argv, __func__);
    //mpy_thread_args* args = alloc_mpy_thread_args(argc, argv);
    //aos_task_new("python_main_entry", python_main_entry, (void*)args, 1024*20);
    mpy_init();
    mpy_run(argc, argv);
}

static struct cli_command command = {
    .name     = "python",
    .help     = "start micropython ",
    .function = handle_identity_cmd
};

int application_start(int argc, char *argv[])
{
    int count = 100;

    int  ret = aos_cli_register_command(&command);
    if (ret) {
        printf("register micropython command failed \r\n");
    }

}


