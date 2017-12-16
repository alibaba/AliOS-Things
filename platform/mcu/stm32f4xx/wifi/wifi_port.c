/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "common.h"
#include "mico_wlan.h"

hal_wifi_module_t sim_aos_wifi_mico;


static int wifi_init(hal_wifi_module_t *m)
{
    printf("wifi init success!!\n");
    mxchipInit();
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
	mico_wlan_get_mac_address(mac);
};


static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    int ret;
    network_InitTypeDef_st conf;

    conf.dhcpMode = init_para->dhcp_mode;
    conf.wifi_mode = init_para->wifi_mode;
    memcpy(conf.wifi_ssid, init_para->wifi_ssid, 32);
    memcpy(conf.wifi_key, init_para->wifi_key, 64);
    memcpy(conf.wifi_ssid, init_para->wifi_ssid, 32);
    memcpy(conf.local_ip_addr, init_para->local_ip_addr, 16);
    memcpy(conf.net_mask, init_para->net_mask, 16);
    memcpy(conf.gateway_ip_addr, init_para->gateway_ip_addr, 16);
    memcpy(conf.dnsServer_ip_addr, init_para->dns_server_ip_addr, 16);
    conf.wifi_retry_interval = init_para->wifi_retry_interval;
	ret = micoWlanStart(&conf);

    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;

 	ret = micoWlanStartAdv((network_InitTypeDef_adv_st*)init_para_adv);
	
    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;
    IPStatusTypedef status;
    wlanInterfaceTypedef iface;
    if (wifi_type == SOFT_AP)
        iface = Soft_AP;
    else
        iface = Station;
	ret = micoWlanGetIPStatus(&status, iface);
    memcpy(out_net_para, &status, sizeof(status));
    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	int ret;
	LinkStatusTypeDef status;
    
    ret = micoWlanGetLinkStatus(&status);
    memcpy(out_stat, &status, sizeof(status));
    
	return ret;
}

static void start_scan(hal_wifi_module_t *m)
{
    micoWlanStartScan();
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	micoWlanStartScanAdv();
}

static int power_off(hal_wifi_module_t *m)
{
    return micoWlanPowerOff();
}

static int power_on(hal_wifi_module_t *m)
{
    return micoWlanPowerOn();
}

static int suspend(hal_wifi_module_t *m)
{
    return micoWlanSuspend();
}

static int suspend_station(hal_wifi_module_t *m)
{
    return micoWlanSuspendStation();
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return micoWlanSuspendSoftAP();
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return mico_wlan_monitor_set_channel(ch);
}

static void start_monitor(hal_wifi_module_t *m)
{
	mico_wlan_start_monitor();
}

static void stop_monitor(hal_wifi_module_t *m)
{
	mico_wlan_stop_monitor();
}

monitor_data_cb_t aos_monitro_cb = NULL;
static void mico_wlan_monitor_cb(uint8_t*data, int len)
{
    hal_wifi_link_info_t info;
    info.rssi = 0;
    if (aos_monitro_cb) {
        aos_monitro_cb(data, len, &info);
    }
}
static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    aos_monitro_cb = fn;
	mico_wlan_register_monitor_cb(mico_wlan_monitor_cb);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return kUnsupportedErr;
}

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
	if (sim_aos_wifi_mico.ev_cb == NULL)
		return;
	if (sim_aos_wifi_mico.ev_cb->ip_got == NULL)
		return;

	sim_aos_wifi_mico.ev_cb->ip_got(&sim_aos_wifi_mico, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
	if (sim_aos_wifi_mico.ev_cb == NULL)
		return;
	if (sim_aos_wifi_mico.ev_cb->para_chg == NULL)
		return;

	sim_aos_wifi_mico.ev_cb->para_chg(&sim_aos_wifi_mico, ap_info, key, key_len, NULL);
}

void WifiStatusHandler(int status)
{
	if (sim_aos_wifi_mico.ev_cb == NULL)
		return;
	if (sim_aos_wifi_mico.ev_cb->stat_chg == NULL)
		return;

	sim_aos_wifi_mico.ev_cb->stat_chg(&sim_aos_wifi_mico, status, NULL);
}

void ApListCallback(hal_wifi_scan_result_t *pApList)
{
	int i;
	
	printf("AP %d: \r\n", pApList->ap_num);
	for(i=0; i<pApList->ap_num; i++) {
		printf("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
	}
	if (sim_aos_wifi_mico.ev_cb == NULL)
		return;
	if (sim_aos_wifi_mico.ev_cb->scan_compeleted == NULL)
		return;

	sim_aos_wifi_mico.ev_cb->scan_compeleted(&sim_aos_wifi_mico, 
		(hal_wifi_scan_result_t*)pApList, NULL);
}

void ApListAdvCallback(hal_wifi_scan_result_adv_t *pApAdvList)
{
	if (sim_aos_wifi_mico.ev_cb == NULL)
		return;
	if (sim_aos_wifi_mico.ev_cb->scan_adv_compeleted == NULL)
		return;

	sim_aos_wifi_mico.ev_cb->scan_adv_compeleted(&sim_aos_wifi_mico, 
		pApAdvList, NULL);
}

void start_debug_mode(hal_wifi_module_t *m)
{
    wifimgr_debug_enable(1);
}

void stop_debug_mode(hal_wifi_module_t *m)
{
    wifimgr_debug_enable(0);
}

hal_wifi_module_t sim_aos_wifi_mico = {
    .base.name           = "sim_aos_wifi_mico",
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
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};

