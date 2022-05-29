/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef AOS_BOARD_HAAS700
#include "amp_platform.h"
#include "aos_network.h"
#include "netmgr.h"
#include "netmgr_wifi.h"
#include <uservice/eventid.h>
#include <uservice/uservice.h>

#ifndef WIFI_DEV_PATH
#define WIFI_DEV_PATH "/dev/wifi0"
#endif

#define LOG_TAG       "aos_wifi"

static int _remap_exceptions(int e)
{
    int32_t err = 0;
    // switch (e) {
    //     case ESP_ERR_WIFI_NOT_INIT:
    //         err = AOS_ERR_WIFI_NOT_INIT;
    //         break;
    //     case ESP_ERR_WIFI_NOT_STARTED:
    //         err = AOS_ERR_WIFI_NOT_STARTED;
    //         break;
    //     case ESP_ERR_WIFI_NOT_STOPPED:
    //         err = AOS_ERR_WIFI_NOT_STOPPED;
    //         break;
    //     case ESP_ERR_WIFI_IF:
    //         err = AOS_ERR_WIFI_IF;
    //         break;
    //     case ESP_ERR_WIFI_MODE:
    //         err = AOS_ERR_WIFI_MODE;
    //         break;
    //     case ESP_ERR_WIFI_STATE:
    //         err = AOS_ERR_WIFI_STATE;
    //         break;
    //     case ESP_ERR_WIFI_CONN:
    //         err = AOS_ERR_WIFI_CONN;
    //         break;
    //     case ESP_ERR_WIFI_NVS:
    //         err = AOS_ERR_WIFI_NVS;
    //         break;
    //     case ESP_ERR_WIFI_MAC:
    //         err = AOS_ERR_WIFI_MAC;
    //         break;
    //     case ESP_ERR_WIFI_SSID:
    //         err = AOS_ERR_WIFI_SSID;
    //         break;
    //     case ESP_ERR_WIFI_PASSWORD:
    //         err = AOS_ERR_WIFI_PASSWORD;
    //         break;
    //     case ESP_ERR_WIFI_TIMEOUT:
    //         err = ETIMEDOUT;
    //         break;
    //     case ESP_ERR_WIFI_WAKE_FAIL:
    //         err = AOS_ERR_WIFI_WAKE_FAIL;
    //         break;
    //     case ESP_ERR_WIFI_WOULD_BLOCK:
    //         err = AOS_ERR_WIFI_WOULD_BLOCK;
    //         break;
    //     case ESP_ERR_WIFI_NOT_CONNECT:
    //         err = AOS_ERR_WIFI_NOT_CONNECT;
    //         break;
    //     case ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
    //         err = AOS_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    //         break;
    //     case ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:
    //         err = AOS_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    //         break;
    //     case ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
    //         err = AOS_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED;
    //         break;
    //     case ESP_ERR_TCPIP_ADAPTER_NO_MEM:
    //         err = ENOMEM;
    //         break;
    // }
    return err;
}


static void wifi_event_cb(uint32_t event_id, const void *param, void *ctx)
{
    int32_t event;
    int wifi_state = 0;
    aos_wifi_manager_t *wifi_manager = NULL;
    if (ctx != NULL) {
        wifi_manager = (aos_wifi_manager_t *)ctx;
    }

    switch (event_id) {
    case EVENT_NETMGR_WIFI_DISCONNECTED:
        wifi_state = AOS_NET_STA_DISCONNECTED;
        break;

    case EVENT_NETMGR_WIFI_CONNECTED:
        wifi_state = AOS_NET_STA_CONNECTED;
        break;

    case EVENT_NETMGR_GOT_IP:
        wifi_state = AOS_NET_STA_CONNECTED;
        break;

    case EVENT_NETMGR_WIFI_CONN_TIMEOUT:
        wifi_state = AOS_NET_STA_STARTED;
        break;

    default:
        event = AOS_NET_STATE_UNKNOWN;
        break;
    }
    if (wifi_manager != NULL) {
        wifi_manager->wifi_state = wifi_state;
    }

    if (wifi_manager->cb != NULL) {
        wifi_manager->cb(wifi_state, ctx);
    }
    return;
}

int aos_wifi_init(aos_wifi_manager_t *wifi_manager)
{
    int ret = 0;
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl >= 0) {
        LOGE(LOG_TAG, "wifi already init by other task\r\n");
    } else {
        LOGE(LOG_TAG, "aos_wifi_init start");
        ret = event_service_init(NULL);
        if (ret != 0) {
            LOGE(LOG_TAG, "event_service_init failed\r\n");
            return ret;
        }
        ret = netmgr_service_init(NULL);
        if (ret != 0) {
            LOGE(LOG_TAG, "netmgr_service_init failed\r\n");
            return ret;
        }

        netmgr_set_auto_reconnect(NULL, false);
        ret = netmgr_wifi_set_auto_save_ap(true);
        if (ret != 0) {
            LOGE(LOG_TAG, "netmgr_wifi_set_auto_save_ap failed\r\n");
            return ret;
        }
    }
    event_subscribe(EVENT_NETMGR_WIFI_DISCONNECTED, wifi_event_cb, wifi_manager);
    event_subscribe(EVENT_NETMGR_WIFI_CONNECTED, wifi_event_cb, wifi_manager);
    event_subscribe(EVENT_NETMGR_WIFI_CONN_TIMEOUT, wifi_event_cb, wifi_manager);
    event_subscribe(EVENT_NETMGR_GOT_IP, wifi_event_cb, wifi_manager);
    return ret;
}

int aos_wifi_deinit(aos_wifi_manager_t *wifi_manager)
{
    // return netmgr_service_deinit();
    return 0;
}

int aos_wifi_start(aos_wifi_manager_t *wifi_manager)
{
    wifi_manager->is_started = true;
    return 0;
}

int aos_wifi_stop(aos_wifi_manager_t *wifi_manager)
{
    return 0;
}

int aos_net_set_ifconfig(aos_ifconfig_info_t *info)
{
    netmgr_hdl_t hdl;
    printf("dev %s open failed\r\n", __func__);
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_set_ifconfig(hdl, info);
}

int aos_net_get_ifconfig(aos_ifconfig_info_t *info)
{
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_get_ifconfig(hdl, info);
}

int aos_wifi_set_msg_cb(netmgr_msg_cb_t cb)
{
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_set_msg_cb(hdl, cb);
}

int aos_wifi_del_msg_cb(netmgr_msg_cb_t cb)
{
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_del_msg_cb(hdl, cb);
}

AOS_NETWORK_TYPE_E aos_get_network_type()
{
    return AOS_NETWORK_WIFI;
}

int aos_wifi_connect(const char *ssid, const char *passwd)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    netmgr_connect_params_t params;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl < 0) {
        LOGE(LOG_TAG, "netmgr_get_dev failed\r\n");
        return -1;
    }

    memset(&params, 0, sizeof(netmgr_connect_params_t));
    params.type = NETMGR_TYPE_WIFI;
    strncpy(params.params.wifi_params.ssid, ssid, sizeof(params.params.wifi_params.ssid) - 1);
    strncpy(params.params.wifi_params.pwd, passwd, sizeof(params.params.wifi_params.pwd) - 1);
    params.params.wifi_params.timeout = 60000;

    ret = netmgr_connect(hdl, &params);
    if (ret != 0) {
        LOGE(LOG_TAG, "netmgr_connect failed. %d\r\n", ret);
        return ret;
    }

    return ret;
}

int aos_wifi_get_info(aos_wifi_info_t *wifi_info)
{
    netmgr_hdl_t hdl;
    netmgr_config_t config;
    netmgr_ifconfig_info_t info;
    int ap_num;
    int used_ap;                                         /**< ap that is used in the array */
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }

    memset(&info, 0, sizeof(info));
    if (netmgr_get_ifconfig(hdl, &info) == -1) {
        return -1;
    }

    memset(&config, 0, sizeof(config));
    if (netmgr_get_config(hdl, &config) == -1) {
        return -1;
    }
    ap_num = config.config.wifi_config.ap_num;
    used_ap = config.config.wifi_config.used_ap;

    if ((ap_num < MAX_AP_CONFIG_NUM) && (used_ap < ap_num)) {
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

int aos_wifi_disconnect()
{
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_disconnect(hdl);
}

int aos_wifi_get_state()
{
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    if (hdl == -1) {
        return -1;
    }
    return netmgr_get_state(hdl);
}

int aos_get_network_status()
{
    int net_status = aos_wifi_get_state();
    return net_status == AOS_NET_STA_GOT_IP ? 1 : 0;
}

#endif
