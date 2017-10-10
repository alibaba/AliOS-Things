/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/network.h>
#ifdef CONFIG_AOS_DDA
#include <stdlib.h>
#include <dda.h>
#endif

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include "umesh.h"
#include "umesh_hal.h"

static void app_delayed_action(void *arg)
{
#ifdef AOS_NETMGR
    netmgr_init();
    netmgr_start(false);
#endif
}

static void app_main_entry(void *arg)
{
    aos_post_delayed_action(1000, app_delayed_action, arg);
    aos_loop_run();
}

int application_start(int argc, char **argv)
{
    const char *mode = argc > 1 ? argv[1] : "";

    aos_set_log_level(AOS_LL_DEBUG);

    if (strcmp(mode, "--mesh-node") == 0) {
#ifdef CONFIG_AOS_DDA
        dda_enable(atoi(argv[argc-1]));
        dda_service_init();
        dda_service_start();
#endif
    }
    else if (strcmp(mode, "--mesh-master") == 0) {
#ifdef CONFIG_AOS_DDM
        ddm_run(argc, argv);
#endif
    }
    else {
        aos_task_new("meshappmain", app_main_entry, NULL, 8192);
    }

    return 0;
}

