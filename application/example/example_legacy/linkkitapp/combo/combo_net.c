/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include <aos/yloop.h>
#include "ulog/ulog.h"
#include "netmgr.h"

#include <breeze_awss.h>
#include "combo_devinfo.h"

extern char awss_running;

breeze_dev_info_t dinfo = {
        .product_id = PRODUCT_ID,
        .product_key = PRODUCT_KEY,
        .product_secret = PRODUCT_SECRET,
        .device_name = DEVICE_NAME,
        .device_secret = DEVICE_SECRET
    };
static breeze_apinfo_t apinfo;

static void apinfo_ready_handler(breeze_apinfo_t *ap)
{
    if (!ap)
        return;

    memcpy(&apinfo, ap, sizeof(apinfo));
    aos_post_event(EV_BZ_COMBO, CODE_COMBO_AP_INFO_READY, (unsigned long)&apinfo);
}

static void linkkit_work(breeze_apinfo_t *info)
{
    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));

    if (!info)
        return;

    strncpy(config.ssid, info->ssid, sizeof(config.ssid) - 1);
    strncpy(config.pwd, info->pw, sizeof(config.pwd) - 1);
    memcpy(config.bssid, info->bssid, ETH_ALEN);
    netmgr_set_ap_config(&config);
    hal_wifi_suspend_station(NULL);
    LOGI("aos_awss", "Will connect WiFi AP: %s", config.ssid);
    netmgr_reconnect_wifi();
}

static void combo_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_BZ_COMBO) {
        return;
    }

    LOG("%s %d", __func__, event->code);

    awss_running = 1;

    if (event->code == CODE_COMBO_AP_INFO_READY) {
        linkkit_work((breeze_apinfo_t *)event->value);
    }
}

int combo_net_init()
{
    aos_register_event_filter(EV_BZ_COMBO, combo_service_event, NULL);
    if (netmgr_start(false) != 1) {
        breeze_awss_init(apinfo_ready_handler, &dinfo);
    }
    return 0;
}

