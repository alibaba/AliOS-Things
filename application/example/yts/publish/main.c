/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include "yunit/yts.h"

#include "cli.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "fs/kv.h"
#include "aos/yloop.h"

static void handle_yts_cmd(char *pwbuf, int blen, int argc, char **argv);

#ifndef YTS_LINUX
static struct cli_command ncmd = {"yts_run", "running yts testcase", handle_yts_cmd};
#endif

static void handle_yts_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    LOG("Starting customer yts test...\r\n");
    yts_run(argc, argv);
}

int application_start(int argc, char **argv)
{
#ifdef YTS_LINUX
    handle_yts_cmd(NULL, 0, argc, argv);
    aos_kv_deinit();
    exit(0);
#else

    aos_cli_register_command(&ncmd);
    LOG("Please enter command via CLI: \r\n");
    aos_loop_run();
    return 0;
#endif

}

