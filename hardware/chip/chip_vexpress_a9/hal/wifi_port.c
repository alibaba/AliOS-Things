/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/wifi.h"
#include "lwip/netif.h"
//#include "gl_upperlayer.h"

#define UNKNOW_CENTER_FREQ  0x0
#define USER_DISCONNECT     0x4

#define WLAN_STATUS_MEDIA_CONNECT               ((uint32_t) 0x4001000BL)
#define WLAN_STATUS_MEDIA_DISCONNECT            ((uint32_t) 0x4001000CL)
#define WLAN_STATUS_MEDIA_DISCONNECT_LOCALLY	((uint32_t) 0x4001000DL)

typedef enum {
    WIFI_STATE_START_CONNECT,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONECT,
} wifi_state;

typedef enum {
    SCAN_STATE_START_SCAN,
    SCAN_STATE_SCAN_DONE,
} scan_state;

hal_wifi_module_t aos_wifi_module_mt8512;
wifi_state eWIFIState;
scan_state eScanState;
//wifi_event_cb_t rWiFiEventCb;

/****************************************************************************
* CALLBACK FUNCTIONS
*****************************************************************************
*/
extern struct netif sta_netif;

static void wifi_stat_chg_event(int stat, void *arg)
{

    hal_wifi_module_t *m = &aos_wifi_module_mt8512;
    ap_list_t *ap = NULL;
    struct netif * net_if = &sta_netif;

    printf("[hk]WiFi HAL %s stat:%d cb:%p\n", __func__, stat, m->ev_cb);

    switch(stat) {
    case WLAN_STATUS_MEDIA_CONNECT:
        {
        netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);

        if (netif_is_up(net_if)) {
            if(dhcp_start(net_if) != 0) {
                printf("dhcp start error\n");
            }
        }

        if (m->ev_cb) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_CONNECTED, NULL);
        }
        if (!arg)
            break;

        ap = (ap_list_t *) arg;
        printf("[hk]Connected to %s[%pM] in ch %d\n", ap->ssid,
                    ap->bssid, ap->channel);
        eWIFIState = WIFI_STATE_CONNECTED;
        break;
        }
    case WLAN_STATUS_MEDIA_DISCONNECT:
    case WLAN_STATUS_MEDIA_DISCONNECT_LOCALLY:
        if (m->ev_cb) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_DISCONNECTED, NULL);
        }
        eWIFIState = WIFI_STATE_DISCONECT;
        break;
    default:
        break;
    }
}

static void wifi_connect_fail(int err, void *arg)
{

}

static void wifi_scan_done(int ap_num, void *arg)
{
    hal_wifi_scan_result_t *scan_result = NULL;
    hal_wifi_module_t *m = &aos_wifi_module_mt8512;

    eScanState = SCAN_STATE_SCAN_DONE;
    printf("[hk]WiFi HAL %s found %d aps cb:%p\n", __func__, ap_num, m->ev_cb);
    scan_result = aos_malloc(sizeof(hal_wifi_scan_result_t));
    if (!scan_result) {
        printf("WiFi HAL %s alloc mem(%zu) fail!\n", __func__, sizeof(hal_wifi_scan_result_t));
        return;
    }

    scan_result->ap_num = ap_num;
    scan_result->ap_list = (ap_list_t *) arg;

    if (m->ev_cb) {
        printf("[hk] wifi module didn't init callback\n");
        m->ev_cb->scan_compeleted(m, scan_result, NULL);
    } else {
        printf("[hk] wifi module didn't init callback\n");
    }

    aos_free(scan_result);
}

/****************************************************************************
* MODULE FUNCTIONS
*****************************************************************************
*/
static int wifi_init(hal_wifi_module_t *m)
{
    #if 0
    printf("[hk]WiFi HAL %s init!\n", __func__);
    rWiFiEventCb.stat_chg = wifi_stat_chg_event;
    rWiFiEventCb.connect_fail = wifi_connect_fail;
    rWiFiEventCb.scan_done = wifi_scan_done;
    mtk_freertos_register_event_cb(&rWiFiEventCb);
    #endif
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    printf("WiFi HAL %s get mac addr!\n", __func__);
    mtk_freertos_get_mac_addr(mac);
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
};

static int wifi_connect(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    printf("WiFi HAL %s start connect!\n", __func__);
    if (eWIFIState == WIFI_STATE_CONNECTED) {
    	printf("wifi was connected!");
	// TODO: disconnect original ssid
	// TODO: need check with same ssid?
    }
    return mtk_freertos_connect(UNKNOW_CENTER_FREQ, init_para->wifi_bssid,
                                init_para->wifi_ssid, strlen(init_para->wifi_ssid));
}

static int set_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *in_net_para,
                       hal_wifi_type_t wifi_type)
{
    printf("WiFi HAL %s mode:%d dhcp:%u ip:%s gate:%s mask:%s dns:%s mac:%s bip:%s\n",
            __func__, wifi_type, in_net_para->dhcp, in_net_para->ip, in_net_para->gate,
            in_net_para->mask, in_net_para->dns, in_net_para->mac, in_net_para->broadcastip);
    return 0;//return mtk_freertos_set_ip_stat(in_net_para->ip, wifi_type);
}

static int start_scan(hal_wifi_module_t *m)
{
    printf("[hk]WiFi HAL %s start scan State:%d!\n", __func__, eScanState);
    //TODO: need check is wifi driver ready first
#if 0
    printf("WiFi HAL %s start scan!\n", __func__);
    if (eScanState == SCAN_STATE_START_SCAN) {

        printf("WiFi HAL %s scan process wasn't finish!\n", __func__);
        return -1;
    }
#endif
    eScanState = SCAN_STATE_START_SCAN;
    uint16_t ch_list[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13};
    return mtk_freertos_scan(ch_list, sizeof(ch_list)/sizeof(ch_list[0]));
}

static int start_specified_scan(hal_wifi_module_t *m, ap_list_t *ap_list, int ap_num)
{
    printf("[hk]WiFi HAL %s specified scan ap_num:%d!\n", __func__, ap_num);
    return mtk_freertos_specified_scan(ap_list, ap_num);
}

static int wlan_disconnect(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s cancel connect!\n", __func__);
    if (eWIFIState != WIFI_STATE_CONNECTED) {

        printf("WiFi HAL %s wlan was already disconnect!\n", __func__);    
        return -1;
    }
    return mtk_freertos_disconnect(NULL, USER_DISCONNECT);
}

static int wlan_cancel(hal_wifi_module_t *m)
{
    return 0;
}

static int wlan_set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void wlan_start_monitor(hal_wifi_module_t *m)
{

    printf("[hk]WiFi HAL %s start monitor!\n", __func__);
    mtk_freertos_start_monitor();
}

static void wlan_stop_monitor(hal_wifi_module_t *m)
{

    printf("[hk]WiFi HAL %s stop monitor!\n", __func__);
    mtk_freertos_stop_monitor();
}

static void wlan_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
}

static void wlan_start_mgnt_monitor(hal_wifi_module_t *m)
{
}

static void wlan_stop_mgnt_monitor(hal_wifi_module_t *m)
{
}

static void wlan_register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

struct netif* wlan_get_netif(hal_wifi_module_t *m, hal_wifi_type_t mode)
{
    return mtk_freertos_get_netif(mode);
}


hal_wifi_module_t aos_wifi_module_mt8512 = {
    .base.name                      = "aos_wifi_module_mt8512",
    .init                           =  wifi_init,
    .get_mac_addr                   =  wifi_get_mac_addr,
    .connect                        =  wifi_connect,
    .set_ip_stat                    =  set_ip_stat,
    .start_scan                     =  start_scan,
    .start_specified_scan           =  start_specified_scan,
    .disconnect                     =  wlan_disconnect,
    .cancel                         =  wlan_cancel,
    .set_channel                    =  wlan_set_channel,
    .start_monitor                  =  wlan_start_monitor,
    .stop_monitor                   =  wlan_stop_monitor,
    .register_monitor_cb            =  wlan_register_monitor_cb,
    .start_mgnt_monitor             =  wlan_start_mgnt_monitor,
    .stop_mgnt_monitor              =  wlan_stop_mgnt_monitor,
    .register_wlan_mgnt_monitor_cb  =  wlan_register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame      =  wlan_send_80211_raw_frame,
    .get_netif                      =  wlan_get_netif,
};
