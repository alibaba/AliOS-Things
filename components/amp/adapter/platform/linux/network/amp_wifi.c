/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_network.h"

int32_t HAL_WiFi_Init()
{
    return 0;
}

int32_t HAL_WiFi_Connect(const char *ssid, const char *passwd)
{
    if (strcmp(ssid, "aiot") == 0 && strcmp(passwd, "iot123456") == 0) {
        return 0;
    }

    return -1;
}

int32_t HAL_WiFi_Get_Info(wifi_info_t *wifi_info)
{
    int32_t ret = -1;

    char ip[4] = {192, 168, 1, 1};
    char mac[6] = {0xaa, 0x12, 0xde, 0xa3, 0x45, 0xfe};

    memset(wifi_info, 0x0, sizeof(wifi_info_t));

    wifi_info->ssid = "aiot";
    wifi_info->rssi = -55;

    memcpy(wifi_info->ip, ip, 4);
    memcpy(wifi_info->mac, mac, 6);

    return 0;
}

int32_t HAL_WiFi_Disconnect()
{
    return 0;
}