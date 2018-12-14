/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"

extern int tiny_engine_start();

/*the entrance for tinyengine on AliOS-things*/
int application_start(int argc, char *argv[])
{
    tiny_engine_start();
    aos_loop_run();
    return 0;
}

