/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <yts.h>
#include <kvmgr.h>

int application_start(int argc, char **argv)
{
    yts_run(argc, argv);
    aos_kv_deinit();
    exit(0);
}

