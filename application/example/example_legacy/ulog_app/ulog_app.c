/*
* Copyright (C) 2015-2019 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "network/network.h"
#include "aos/vfs.h"

#ifdef ULOG_MOD
#undef ULOG_MOD
#endif /* ULOG_MOD */

#define ULOG_MOD "ULOG_DEMO"
#define TEST_STR  "this is ulog tester"

static char show_log_list_buffer[512];

static void ulog_test(void *paras);

#ifdef AOS_COMP_NETMGR
#include "network/network.h"
#include "netmgr.h"
#include "aos/yloop.h"

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    LOG("wifi event %d %d", event->type, event->code);
    if (EV_WIFI==event->type && CODE_WIFI_ON_GOT_IP==event->code) {
        struct in_addr  listen_addr;
        char buf[24];
        netmgr_ap_config_t config;
        memset(&config, 0, sizeof(netmgr_ap_config_t));
        netmgr_get_ap_config(&config);
        LOG("wifi_service_event config.ssid %s", config.ssid);
#ifdef ULOGAPP_CONFIG_UDP
        /* use below statements to pass the ip address of syslog watcher */        
        inet_aton("192.168.43.19", &listen_addr);        
        snprintf(buf, 24, "listen ip=%d", listen_addr.s_addr);
        ulog_man(buf);

        /* use below statements to notify ulog that the net is working */
        ulog_man("tcpip service=1");
#endif
    }
}
#endif

int application_start(int argc, char **argv)
{
    /* set ulog level, make all the level of log is not lower than this value could be logged */
    aos_set_log_level(AOS_LL_INFO);
#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

#ifdef AOS_COMP_NETMGR
    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);
#endif

    aos_task_new("ulog_test", ulog_test, NULL, 2048);

#ifdef AOS_COMP_NETMGR
    aos_loop_run();
#else
    while (1) {
        aos_msleep(1000);
    }
#endif

    return 0;
}

static void ulog_test(void *paras)
{
    int i = 0;
    while (1) {
        LOGI(ULOG_MOD, "%d-%s", i, TEST_STR);
        i++;
        if(10==i) {
#if ULOG_CONFIG_POP_FS
            if(0==aos_get_ulog_list(show_log_list_buffer, sizeof(show_log_list_buffer))){
                LOG("Log List %s", show_log_list_buffer);
            }
#endif
        }
        aos_msleep(500);
    }
}
