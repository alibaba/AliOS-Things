/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#define _BE_OSAL_NET_C

#include <aos/aos.h>
#include <be_osal_net.h>
#include <hal/wifi.h>
#include <netmgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/be_common.h>

static bool s_wifi_initlized = false;
static be_osal_wifi_scan_cb p_wifi_scan_cb;

#define MOD "osal_net"
#define MAX_SCAN_NUM 10
#define CHECK_IF_WIFI_INITLIZED()                               \
    do {                                                        \
        if (s_wifi_initlized == false) return BE_WIFI_NOT_INIT; \
    } while (0)

static void be_hal_wifi_start(char *ssid, char *pw) 
{
    hal_wifi_init_type_t type;

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    strncpy(type.wifi_key, pw, sizeof(type.wifi_key) - 1);
    hal_wifi_start(NULL, &type);
}

static int be_wifi_scan_callback(const char ssid[MAX_SSID_LEN],
                                        const uint8_t bssid[ETH_ALEN],
                                        enum NETMGR_AWSS_AUTH_TYPE auth,
                                        enum NETMGR_AWSS_ENC_TYPE encry,
                                        uint8_t channel, char rssi, int is_last_ap) 
{
    static int s_index = 0;
    int i = 0;
    be_wifi_scan_result_t *p_result[MAX_SCAN_NUM];

    be_debug(
        MOD,
        "%s,%d: "
        "index=%d,ssid=%s,bssid=%s,auth=%d,channel=%d,rssi=%d,is_last=%d\n\r",
        __FUNCTION__, __LINE__, s_index, ssid, bssid, auth, channel, rssi,
        is_last_ap);

    if (s_index < MAX_SCAN_NUM) {
        p_result[s_index] =
            (be_wifi_scan_result_t *)malloc(sizeof(be_wifi_scan_result_t));
        if (p_result[s_index] != NULL) {
            memset(p_result[s_index], 0, sizeof(be_wifi_scan_result_t));
            strncpy(p_result[s_index]->ssid, ssid, MAX_SSID_LEN - 1);
            strncpy(p_result[s_index]->bssid, bssid, ETH_ALEN - 1);
            p_result[s_index]->channel = (char)channel;
            p_result[s_index]->encry = (int)encry;
            p_result[s_index]->rssi = (char)rssi;
            p_result[s_index]->auth = (int)auth;
            s_index++;
        }
    }

    if ((is_last_ap == 1) && (s_index < MAX_SCAN_NUM) &&
        p_wifi_scan_cb != NULL) {
        p_result[s_index] = NULL;  // make sure this is the last index
        p_wifi_scan_cb(&p_result);
        p_wifi_scan_cb = NULL;
        s_index = 0;
    }

    // fre all malloc buffer
    if ((is_last_ap == 1) || s_index >= MAX_SCAN_NUM) {
        s_index = 0;
        for (i = 0; i < MAX_SCAN_NUM; i++) {
            if (p_result[i] == NULL) {
                break;
            }
            free(p_result[i]);
        }
    }

    return 0;
}

be_wifi_ret_t be_osal_wifi_scan(be_osal_wifi_scan_cb cb) 
{
    CHECK_IF_WIFI_INITLIZED();
    if (cb == NULL) {
        return BE_WIFI_PARM_ERR;
    }

    p_wifi_scan_cb = cb;
    be_debug(MOD, "%s enter\n\r", __FUNCTION__);
    netmgr_register_wifi_scan_result_callback(
        (netmgr_wifi_scan_result_cb_t)be_wifi_scan_callback);
    hal_wifi_start_scan(NULL);
    return BE_WIFI_RET_OK;
}

int be_osal_wifi_init() 
{
    int ret = -1;
    bool autoconfig = false;
    netmgr_ap_config_t *p_wifi_config = NULL;

    netmgr_init();
    netmgr_start(autoconfig);
    aos_msleep(100);
    return ret;
}

// reconnect wifi many times should call this first
void be_osal_wifi_suspend(void) 
{
    hal_wifi_module_t *module;

    module = hal_wifi_get_default_module();
    hal_wifi_suspend_station(module);
}

int be_osal_wifi_connect(be_osal_wifi_config_t *config) 
{
    bool auto_netmgr = false;
    netmgr_ap_config_t apconfig;

    be_debug(MOD, "%s:ssid=%s\n\r", __FUNCTION__, config->ssid);
    memset(&apconfig, 0, sizeof(apconfig));
    strcpy(apconfig.ssid, config->ssid);
    strcpy(apconfig.pwd, config->pwd);
    netmgr_set_ap_config(&apconfig);
    netmgr_start(auto_netmgr);

    return 0;
}

int be_osal_wifi_get_ip(char *ip) 
{
    netmgr_wifi_get_ip(ip);
    return 0;
}

int be_osal_wifi_save_config(be_osal_wifi_config_t *config)
{
    int ret = -1;
    
    #define NETMGR_WIFI_KEY "wifi"

    if ( (NULL == config) || !strlen(config->ssid) || !strlen(config->pwd) ) {
        return -1;
    }

    ret = aos_kv_set(NETMGR_WIFI_KEY, (char *)config, sizeof(be_osal_wifi_config_t), 1);
    return ret;
}

#undef _BE_OSAL_NET_C
