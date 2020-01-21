/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include <umesh.h>

static void app_delayed_action(void *arg)
{
    node_mode_t mode;

#ifdef AOS_NETMGR
    /**
     * Initialize network interface
     */
    netmgr_init();
#endif

    /**
     * Initialize uMesh
     */
    umesh_init(MODE_RX_ON);

    /**
     * set node mode to normal mode
     */
    mode = umesh_get_mode() & (~MODE_LEADER);
    umesh_set_mode(mode);

    /**
     * start uMesh
     */
    umesh_start(NULL);
}

static void app_main_entry(void *arg)
{
    aos_post_delayed_action(1000, app_delayed_action, arg);
    aos_loop_run();
}

int application_start(int argc, char **argv)
{
    aos_set_log_level(AOS_LL_DEBUG);

    aos_task_new("meshappmain", app_main_entry, NULL, 8192);
    return 0;
}
