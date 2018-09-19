/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <yts.h>
#include <kvmgr.h>
#include <aos/aos.h>

int application_start(int argc, char **argv)
{
    LOG("Starting customer yts test...\r\n");
    yts_run(0, NULL);
    aos_loop_run();
    return 0;
}