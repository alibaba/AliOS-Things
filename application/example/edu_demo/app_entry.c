/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <k_api.h>
#include "aos/cli.h"
#include "aos/kernel.h"
#include "uagent/uagent.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "k1_apps/menu.h"
#include "hal_oled.h"

#include "netmgr.h"
#include "app_entry.h"
#include "build_version.h"

extern int board_test(void);

int wifi_connected = 0;
int bt_connected = 0;
int get_ipaddr = 0;
int battery_level = 0;
char edu_ip_addr[17];

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    switch(event->code)
    {
        case CODE_WIFI_ON_DISCONNECT:
        wifi_connected = 0;
        get_ipaddr = 0;
        break;

        case CODE_WIFI_ON_CONNECTED:
        wifi_connected = 1;
        break;

        case CODE_WIFI_ON_GOT_IP:
        wifi_connected = 1;
        get_ipaddr = 1;
        break;

        default:
        break;
    }
}

static void start_netmgr(void *p)
{
    FILE *fp;
    FILE *fp_temp;

    LOG("%s\n", __func__);
    aos_msleep(2000);

    fp = fopen("/data/wifi.conf", "r");
    fp_temp = fopen("/data/temp/temp.conf", "r");
    if (fp || fp_temp) {
        netmgr_start(true);
    } else {
        netmgr_start(false);
    }
    aos_task_exit(0);
}

int application_start(int argc, char **argv)
{
    uint8_t value = 0;
    int len = 1;
#ifdef ENABLE_FACTORY_TEST
    if( 0 == aos_kv_get("factory_test", &value, &len) ) {
        printf("factory test entry here!\r\n");
        board_test();
        while(1) {
            aos_msleep(1000);
        }
    } else
#endif
    {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
        aos_post_delayed_action(5000, duration_work, NULL);
#endif
#ifdef CSP_LINUXHOST
        signal(SIGPIPE, SIG_IGN);
#endif
        //aos_set_log_level(AOS_LL_DEBUG);
#if defined(ENABLE_AOS_OTA)
        //ota_register_cb(&ctx, OTA_CB_ID_UPGRADE, (void*)ota_upgrade_cb);
#endif

#ifndef CONFIG_ENABLE_LINKKIT
        netmgr_init();
        netmgr_start(true);
        aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
#else
        linkit_app_start();
#endif
        printf("*****\r\n\r\n\r\nKevin, image version = %s\r\n\r\n",BUILD_VERSION);
        sh1106_init();
        menu_init();

        aos_loop_run();

        return 0;
    }
}