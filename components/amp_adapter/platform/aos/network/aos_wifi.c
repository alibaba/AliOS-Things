/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos_platform.h"
#include "aos_network.h"
#include "netmgr.h"
#include "netmgr_wifi.h"

#define WIFI_DEV_PATH "/dev/wifi0"

int amp_wifi_init()
{
    return netmgr_service_init(NULL);
}


int amp_wifi_set_ifconfig(netmgr_ifconfig_info_t* info) {
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_set_ifconfig(hdl, info);
}

int amp_wifi_get_ifconfig(netmgr_ifconfig_info_t* info) {
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_get_ifconfig(hdl, info);
}

int amp_wifi_set_msg_cb(netmgr_msg_cb_t cb) {
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_get_state(hdl);
}

int amp_wifi_del_msg_cb(netmgr_msg_cb_t cb) {
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_del_msg_cb(hdl, cb);
}

AMP_NETWORK_TYPE_E amp_get_network_type()
{
    return AMP_NETWORK_WIFI;
}

int amp_wifi_connect(const char *ssid, const char *passwd)
{
    netmgr_hdl_t hdl;
    netmgr_connect_params_t params;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }

    memset(&params, 0, sizeof(netmgr_connect_params_t));

	strncpy(params.params.wifi_params.ssid, ssid, sizeof(params.params.wifi_params.ssid) - 1);
	strncpy(params.params.wifi_params.pwd, passwd, sizeof(params.params.wifi_params.pwd) - 1);
    params.params.wifi_params.timeout = 60000;
	netmgr_connect(hdl, &params);

    return 0;
}

int amp_get_wifi_info(amp_wifi_info_t *wifi_info)
{
    netmgr_hdl_t hdl;
    netmgr_config_t config;
    netmgr_ifconfig_info_t info;
    int ap_num;
    int used_ap;                                         /**< ap that is used in the array */

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }

	memset(&info, 0, sizeof(info));
    if(netmgr_get_ifconfig(hdl, &info) == -1) {
        return -1;
    }

	memset(&config, 0, sizeof(config));
    if(netmgr_get_config(hdl, &config) == -1) {
        return -1;
    }
    ap_num = config.config.wifi_config.ap_num;
    used_ap = config.config.wifi_config.used_ap;

    if((ap_num < MAX_AP_CONFIG_NUM) && (used_ap < ap_num)) {
	    memset(wifi_info->ssid, 0, sizeof(wifi_info->ssid));
	    strncpy(wifi_info->ssid, config.config.wifi_config.config[used_ap].ssid, sizeof(wifi_info->ssid) - 1);
    } else {
        return -1;
    }

    snprintf(wifi_info->ip, sizeof(wifi_info->ip), "%s", info.ip_addr);
	memcpy(wifi_info->mac, info.mac, sizeof(wifi_info->mac));
	wifi_info->rssi = info.rssi;

    return 0;
}

int amp_wifi_disconnect()
{
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_disconnect(hdl);
}

int amp_wifi_get_state() {
    netmgr_hdl_t hdl;

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        return -1;
    }
    return netmgr_get_state(hdl);
}

