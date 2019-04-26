/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "netmgr.h"

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

#ifdef NET_WITH_WIFI
int32_t netmgr_connect(const char *ssid, const uint8_t *password)
{
    netmgr_ap_config_t config;

    memset(&config, 0, sizeof(config));
    if (ssid == NULL || strlen(ssid) > MAX_SSID_SIZE ||
        password == NULL  || strlen(password) > MAX_PWD_SIZE) {
        return -1;
    }

    strncpy(config.ssid, ssid, strlen(ssid));
    strncpy(config.pwd, password, strlen(password));
    netmgr_set_ap_config(&config);
    netmgr_wifi_start(false);
    return 0;
}

#endif
