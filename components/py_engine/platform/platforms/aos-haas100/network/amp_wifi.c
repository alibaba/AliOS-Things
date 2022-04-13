/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_network.h"
#include "netmgr.h"
#include "netmgr_wifi.h"


int32_t HAL_WiFi_Init()
{
    return wifi_service_init();
}

int32_t HAL_WiFi_Connect(const char *ssid, const char *passwd)
{
#if 0
	netmgr_ap_config_t config;

	memset(&config, 0, sizeof(config));
	strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
	strncpy(config.pwd, passwd, sizeof(config.pwd) - 1);
	netmgr_set_ap_config(&config);
	netmgr_start(false);
#else
	// wifi_service_wifi_connect(ssid, passwd, NULL, 60000);
#endif
	return 0;
}

int32_t HAL_WiFi_Get_Info(wifi_info_t *wifi_info)
{
    // char ip[32];
    // netmgr_ap_config_t config;
	// netmgr_stats_t stat;

	// memset(&stat, 0, sizeof(stat));
	// netmgr_stats(INTERFACE_WIFI, &stat);
	// if (!stat.ip_available)
	// 	return -1;
	// snprintf(wifi_info->ip, sizeof(wifi_info->ip), "%s", stat.ip);

	// memset(&config, 0, sizeof(config));
	// netmgr_get_ap_config(&config);
	// wifi_info->ssid = aos_malloc(strlen(config.ssid) + 1);
	// if (!wifi_info->ssid) {
	// 	return -1;
	// }
	// memset(wifi_info->ssid, 0, strlen(config.ssid) + 1);
	// memcpy(wifi_info->ssid, config.ssid, strlen(config.ssid));

	// wifi_service_get_mac(wifi_info->mac);
	// wifi_service_get_rssi(&wifi_info->rssi);

    return 0;
}

int32_t HAL_WiFi_Disconnect()
{
    // return wifi_service_disconnect();
	return 0;
}
