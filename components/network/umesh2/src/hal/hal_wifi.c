/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <hal/wifi.h>
#include "define.h"
#include "netmgr_wifi.h"
#include "hal_wifi.h"

int umesh_wifi_send(const uint8_t *buf, int len) {
	int ret;
    ret = hal_wlan_send_80211_raw_frame(hal_wifi_get_default_module(), (uint8_t *)buf, len);
    if(ret != 0) {
        ret = UMESH_WIFI_RAW_SEND_FAILED;
    }
	return ret;
}

int umesh_wifi_get_mac(uint8_t mac_str[IEEE80211_MAC_ADDR_LEN])
{
    if(mac_str == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    int ret = hal_wifi_get_mac_addr(NULL, mac_str);
    if(ret != 0) {
        ret = UMESH_WIFI_GET_MAC_FAILED;
    }
    return ret;
}

int  umesh_wifi_set_channel(int channel)
{
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    int ret = hal_wifi_set_channel(module, channel);
    if(ret != 0) {
        ret =  UMESH_WIFI_SET_CHAN_FAILED;
    }
    return ret;
}
int  umesh_wifi_get_channel()
{
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    int ret = hal_wifi_get_channel(module);
    return ret;
}

int umesh_wifi_get_channel_list(const uint8_t **p, int *num){
 
    if(p == NULL || num == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    static uint8_t chan_list[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};

    hal_wifi_module_t *module = hal_wifi_get_default_module();
    *num = sizeof(chan_list);
    *p = chan_list;
    return 0;
} 

// int umesh_wifi_get_channel_list(const uint8_t **p, int *num){
 
//     if(p == NULL || num == NULL) {
//         return UMESH_ERR_NULL_POINTER;
//     }
//     hal_wifi_module_t *module = hal_wifi_get_default_module();
//     printf("before get chan list\r\n");
//     *num = hal_wifi_get_channel_list(module, p);
//     printf("after get chan list\r\n");
//     if(*num == NULL) {
//         return UMESH_WIFI_NO_CHAN_LIST;
//     }
//     return 0;
// } 

int umesh_wifi_get_connect_state()
{
    return netmgr_wifi_get_ip_state() ? 1 : 0;
}


static umesh_wifi_frame_cb_t monitor_cb = NULL;
static void * monitor_context = NULL;
static void mgnt_rx_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    if (monitor_cb) {
        monitor_cb(data, len, info->rssi, monitor_context);
    }
}

int umesh_wifi_register_monitor_cb(uint32_t filter_mask, umesh_wifi_frame_cb_t callback, void *context)
{
    monitor_cb = callback;
    monitor_context = context;
    if (callback != NULL) {
        
        hal_wlan_register_mgnt_monitor_cb(NULL, mgnt_rx_cb);
    } else {
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    }

    return 0;
}
