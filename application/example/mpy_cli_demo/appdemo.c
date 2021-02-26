/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/cli.h"
#include "board.h"
#include <k_api.h>
#include "haascv_main.h"

static void handle_identity_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    mpy_init();
    mpy_run(argc,argv);
}

static struct cli_command command = {
    .name     = "micropython",
    .help     = "start micropython ",
    .function = handle_identity_cmd
};

int application_start(int argc, char *argv[])
{
    int  ret = aos_cli_register_command(&command);
    if (ret) {
        printf("register micropython command failed \n");
    }else{
        printf("register micropython command succeed \n");
    }
}
