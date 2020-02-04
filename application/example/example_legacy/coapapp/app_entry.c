/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/*!
 * @file app_entry.c
 *
 * This file includes sample code of start CoAP example task after
 * got ip event.
 *
 * Use CLI command "netmgr connect ssid password" to start WiFi connection
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "netmgr.h"
#include "app_entry.h"

#ifdef WITH_SAL
#include <atcmd_config_module.h>
#endif

#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

static char linkkit_started = 0;

typedef void (*task_fun)(void *);

/*
 * WiFi Got IP event
 */
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        return;
    }

    /* Start CoAP task  */
    if (!linkkit_started) {
        aos_task_new("iotx_example", (task_fun)linkkit_main, NULL, 1024*6);
        linkkit_started = 1;
    }
}


int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    /* Set ulog ouput level to DEBUG */
    aos_set_log_level(AOS_LL_DEBUG);

    /* Register WiFi event handle */
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    /* Initialize and start net manager */
    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
