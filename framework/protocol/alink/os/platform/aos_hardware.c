/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <hal/hal.h>

#include "platform.h"
#include "platform_config.h"

char *platform_get_multicast_ifname(char *ifname, int ifname_size)
{
    return NULL;
}

char *platform_wifi_get_mac(char mac_str[PLATFORM_MAC_LEN])
{
    uint8_t mac[6];

    hal_wifi_get_mac_addr(NULL, mac);

    snprintf(mac_str, PLATFORM_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
}

int platform_wifi_get_rssi_dbm(void)
{
    int ret;

    hal_wifi_link_stat_t stat;

    ret = hal_wifi_get_link_stat(NULL, &stat);
    if (ret != 0) {
        return -1;
    }

    return stat.wifi_strength;
}

uint32_t platform_wifi_get_ip(char ip_str[PLATFORM_IP_LEN])
{
    int i;
    int ret;

    hal_wifi_ip_stat_t ip_stat;

    ret = hal_wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret != 0) {
        return 1;
    }

    for (i = 0; i < PLATFORM_IP_LEN; i++) {
        ip_str[i] = ip_stat.ip[i];
    }

    return 0;
}

char *platform_get_chipid(char chipid[PLATFORM_CID_LEN])
{
    strncpy(chipid, "emw3006", PLATFORM_CID_LEN);
    chipid[PLATFORM_CID_LEN - 1] = '\0';
    return chipid;
}

char *platform_get_os_version(char os_ver[STR_SHORT_LEN])
{
    strncpy(os_ver, "aos", STR_SHORT_LEN);
    os_ver[STR_SHORT_LEN - 1] = '\0';
    return os_ver;
}

int platform_config_write(const char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_set("alink", buffer, length, 1);
}

int platform_config_read(char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_get("alink" , buffer, &length);
}

int platform_sys_net_is_ready(void)
{
    int ret;

    hal_wifi_link_stat_t link_stat;

    ret = hal_wifi_get_link_stat(NULL, &link_stat);
    if (ret != 0) {
        return 0;
    }

    return !!link_stat.is_connected;
}

void platform_sys_reboot(void)
{
    aos_reboot();
}

char *platform_get_module_name(char name_str[STR_SHORT_LEN])
{
    return name_str;
}

const char *platform_get_storage_directory(void)
{
    return NULL;
}
