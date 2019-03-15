/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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

#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

static char linkkit_started = 0;

static app_main_paras_t entry_paras;

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
        //clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
        aos_task_new("iotx_example",linkkit_main,(void *)&entry_paras,1024*6);
        linkkit_started = 1;
    }
}

#ifdef TEST_LOOP
const  char *input_data[]= {"coapapp","-e","online","-l","-s","dtls"};
#endif
int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef TEST_LOOP
    argc = 6;
    argv = input_data;
#endif    
    entry_paras.argc = argc;
    entry_paras.argv = argv;

#ifdef WITH_SAL
    sal_init();
#endif
    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);

    aos_loop_run();

    return 0;
}
