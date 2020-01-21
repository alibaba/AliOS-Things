/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "netmgr.h"

extern int jsengine_main(void);

/* the entrance for JSEngine on AliOS-things */
int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);

    jsengine_main();

    aos_loop_run();

    return 0;
}

