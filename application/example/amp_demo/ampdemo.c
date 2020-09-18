/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "netmgr.h"

#define AMP_TASK_STACK_SIZE 4096
#define AMP_TASK_PRIO (AOS_DEFAULT_APP_PRI - 4)

aos_task_t g_amp_task;

extern int amp_main();

static int32_t amp_service_start()
{
    amp_main();
}

int application_start(int argc, char *argv[])
{
    LOG("AliOS Things MiniProgram!\n");

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();
    netmgr_start(0);

    amp_service_start();

    aos_loop_run();
    return 0;
}
