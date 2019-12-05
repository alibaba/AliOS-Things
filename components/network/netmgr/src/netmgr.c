/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <netmgr.h>
#include "hal/net.h"
#include "aos/kernel.h"
#include "netmgr_priv.h"

static net_interface_t g_net_interfaces[INTERFACE_INDEX_MAX];

int netmgr_init(void)
{
    int ret = -1;

#ifdef NET_WITH_WIFI
    ret = netmgr_wifi_init();
    if (ret < 0) {
        return ret;
    }
#endif

#ifdef NET_WITH_CELLULAR
    ret = netmgr_cellular_init();
    if (ret < 0) {
        return ret;
    }
#endif

    return ret;
}

void netmgr_deinit(void)
{
#ifdef NET_WITH_WIFI
    netmgr_wifi_deinit();
#endif

#ifdef NET_WITH_CELLULAR
    netmgr_cellular_deinit();
#endif
}

int netmgr_start(bool autoconfig)
{
    int ret = -1;

#ifdef NET_WITH_WIFI
    ret = netmgr_wifi_start(autoconfig);
    if (ret < 0) {
        return ret;
    }
#endif

#ifdef NET_WITH_CELLULAR
    ret = netmgr_cellular_start();
    if (ret < 0) {
        return ret;
    }
#endif

    return ret;
}

int netmgr_stats(int32_t interface, netmgr_stats_t *stats)
{
    if (stats == NULL) {
        return -1;
    }

    if (interface < 0 || interface >= INTERFACE_INDEX_MAX) {
        return -1;
    }

    memset(stats, 0, sizeof(netmgr_stats_t));
    switch (g_net_interfaces[interface].interface_type) {
        case INTERFACE_WIFI:
#ifdef NET_WITH_WIFI
            stats->ip_available = netmgr_wifi_get_ip_state();
            netmgr_wifi_get_ip(stats->ip);
#endif
            break;
        case INTERFACE_CELLULAR:
#ifdef NET_WITH_CELLULAR
            stats->ip_available = netmgr_cellular_get_ip_state();
            netmgr_cellular_get_ip(stats->ip);
#endif
            break;
        default:
            return -1;
    }

    return 0;
}

net_interface_t *netmgr_get_net_interface(uint8_t interface_type)
{
    net_interface_t *interface = NULL;

    switch (interface_type) {
        case INTERFACE_WIFI:
#ifdef NET_WITH_WIFI
            interface = &g_net_interfaces[INTERFACE_WIFI];
#endif
            break;
        case INTERFACE_CELLULAR:
#ifdef NET_WITH_CELLULAR
            interface = &g_net_interfaces[INTERFACE_CELLULAR];
#endif
            break;
        default:
            break;
    }

    return interface;
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
        if (netmgr_wifi_get_ip_state() == false) {
            aos_msleep(500);
            time_elapsed += 500;
        } else {
            return 0;
        }
    }

    return -2;
}
#endif
