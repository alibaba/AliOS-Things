/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <netmgr.h>
#include "aos/kernel.h"

int netmgr_init(void)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_init();
#elif defined(NET_WITH_CELLULAR)
    return netmgr_cellular_init();
#endif
}

void netmgr_deinit(void)
{
#ifdef NET_WITH_WIFI
    netmgr_wifi_deinit();
#elif defined(NET_WITH_CELLULAR)
    netmgr_cellular_deinit();
#endif
}

int netmgr_start(bool autoconfig)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_start(autoconfig);
#elif defined(NET_WITH_CELLULAR)
    return netmgr_cellular_start(autoconfig);
#endif
}

void netmgr_stats(int32_t interface, netmgr_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }

    memset(stats, 0, sizeof(netmgr_stats_t));
    stats->ip_available = netmgr_get_ip_state();
    netmgr_wifi_get_ip(stats->ip);
}

#ifdef NET_WITH_WIFI
int32_t netmgr_connect(const char *ssid, const uint8_t *password, uint32_t timeout)
{
    netmgr_ap_config_t config;
    uint32_t time_elapsed = 0;

    memset(&config, 0, sizeof(config));
    if (ssid == NULL || strlen(ssid) > MAX_SSID_SIZE ||
        password == NULL  || strlen(password) > MAX_PWD_SIZE) {
        return -1;
    }

    strncpy(config.ssid, ssid, strlen(ssid));
    strncpy(config.pwd, password, strlen(password));
    netmgr_set_ap_config(&config);
    netmgr_reconnect_wifi();

    while (time_elapsed < timeout) {
        if (netmgr_get_ip_state() == false) {
            aos_msleep(500);
            time_elapsed += 500;
        } else {
            return 0;
        }
    }

    return -2;
}
#endif
