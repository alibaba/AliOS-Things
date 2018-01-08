/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "stm32_wifi.h"

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE
#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

hal_wifi_module_t sim_aos_wifi_stm32l475;

void NetCallback(hal_wifi_ip_stat_t *pnet);
void WifiStatusHandler(int status);

static int wifi_init(hal_wifi_module_t *m)
{
    /* init wifi*/
    char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
    uint8_t mac[6];
    WIFI_Status_t wifi_res = WIFI_Init();
    if (WIFI_STATUS_OK != wifi_res )
    {
        printf("Failed to initialize WIFI module\n");
        return -1;
    }
    /* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
    WIFI_GetModuleName(moduleinfo);
    printf("Module initialized successfully: %s",moduleinfo);

    WIFI_GetModuleID(moduleinfo);
    printf(" %s",moduleinfo);

    WIFI_GetModuleFwRevision(moduleinfo);
    printf(" %s\n",moduleinfo);

    printf("Retrieving the WiFi module MAC address:");
    wifi_res = WIFI_GetMAC_Address( (uint8_t*)mac);
    if ( WIFI_STATUS_OK == wifi_res)
    {
        printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]);
    }
    else
    {
        printf("Failed to get MAC address\n");
    }

    printf("wifi init success!!\n");
    return 0;
};


static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    WIFI_Status_t wifi_res = WIFI_GetMAC_Address(mac);
    if ( WIFI_STATUS_OK != wifi_res) {
        printf("Failed to get MAC address\n");
    }
};


static void connet_wifi_ap(void *arg)
{
    WIFI_Status_t wifi_res;
    int connect_counter = 0;
    uint8_t ip_address[4];
    hal_wifi_ip_stat_t wifi_pnet;
    hal_wifi_init_type_t *init_para = (hal_wifi_init_type_t *)arg;

    wifi_res = WIFI_Disconnect();
    if (WIFI_STATUS_OK != wifi_res) {
        printf("WIFI_Disconnect failed\n");
    }

    do {
        printf("Connecting to AP: %s  Attempt %d/%d ...\n", init_para->wifi_ssid, ++connect_counter, WIFI_CONNECT_MAX_ATTEMPT_COUNT);
        wifi_res = WIFI_Connect(init_para->wifi_ssid, init_para->wifi_key, init_para->access_sec);
        if (wifi_res == WIFI_STATUS_OK) break;
    } while (connect_counter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);

    /* Slight delay since the module seems to take some time prior to being able
        to retrieve its IP address after a connection. */
    aos_msleep(500);
    if (wifi_res == WIFI_STATUS_OK) {
        WifiStatusHandler(1);
        printf("\nConnected to AP %s\n", init_para->wifi_ssid);
    } else {
        WifiStatusHandler(2);
        printf("\nFailed to connect to AP %s\n", init_para->wifi_ssid);
        return;
    }
   
    if ( WIFI_STATUS_OK != WIFI_GetIP_Address(ip_address) ) {
        printf("Fail to get IP address\n");
    } else {
        snprintf(wifi_pnet.ip, sizeof(wifi_pnet.ip), "%d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        printf("Get IP Address: %s\n", wifi_pnet.ip);
        NetCallback(&wifi_pnet);
    }
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (NULL == m || NULL == init_para) {
        printf("wifi_start: invalid parameter\n");
        return -1;
    }

    connet_wifi_ap((void *)init_para);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    return;
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    return;
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{
    return;
}

static void stop_monitor(hal_wifi_module_t *m)
{
    return;
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
    if (sim_aos_wifi_stm32l475.ev_cb == NULL)
        return;
    if (sim_aos_wifi_stm32l475.ev_cb->ip_got == NULL)
        return;

    sim_aos_wifi_stm32l475.ev_cb->ip_got(&sim_aos_wifi_stm32l475, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
    if (sim_aos_wifi_stm32l475.ev_cb == NULL)
        return;
    if (sim_aos_wifi_stm32l475.ev_cb->para_chg == NULL)
        return;

    sim_aos_wifi_stm32l475.ev_cb->para_chg(&sim_aos_wifi_stm32l475, ap_info, key, key_len, NULL);
}

void WifiStatusHandler(int status)
{
    if (sim_aos_wifi_stm32l475.ev_cb == NULL)
        return;
    if (sim_aos_wifi_stm32l475.ev_cb->stat_chg == NULL)
        return;

    sim_aos_wifi_stm32l475.ev_cb->stat_chg(&sim_aos_wifi_stm32l475, (hal_wifi_event_t)status, NULL);
}

void ApListCallback(hal_wifi_scan_result_t *pApList)
{
    int i;
    
    printf("AP %d: \r\n", pApList->ap_num);
    for(i=0; i<pApList->ap_num; i++) {
        printf("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
    }
    if (sim_aos_wifi_stm32l475.ev_cb == NULL)
        return;
    if (sim_aos_wifi_stm32l475.ev_cb->scan_compeleted == NULL)
        return;

    sim_aos_wifi_stm32l475.ev_cb->scan_compeleted(&sim_aos_wifi_stm32l475, 
        (hal_wifi_scan_result_t*)pApList, NULL);
}

void ApListAdvCallback(hal_wifi_scan_result_adv_t *pApAdvList)
{
    if (sim_aos_wifi_stm32l475.ev_cb == NULL)
        return;
    if (sim_aos_wifi_stm32l475.ev_cb->scan_adv_compeleted == NULL)
        return;

    sim_aos_wifi_stm32l475.ev_cb->scan_adv_compeleted(&sim_aos_wifi_stm32l475, 
        pApAdvList, NULL);
}

hal_wifi_module_t sim_aos_wifi_stm32l475 = {
    .base.name           = "sim_aos_wifi_stm32l475",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};
