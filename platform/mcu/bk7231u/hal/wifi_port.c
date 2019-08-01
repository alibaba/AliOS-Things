/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "wlan_ui_pub.h"

hal_wifi_module_t sim_aos_wifi_beken;


static int wifi_init(hal_wifi_module_t *m)
{
    printf("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
	bk_wifi_get_mac_address(mac);
}

void wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
	bk_wifi_set_mac_address(mac);
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    int ret;
	network_InitTypeDef_st network_init;

	os_memset(&network_init, 0, sizeof(network_InitTypeDef_st));
	network_init.wifi_mode = init_para->wifi_mode;
	os_strcpy(network_init.wifi_ssid, init_para->wifi_ssid);
	os_strcpy(network_init.wifi_key, init_para->wifi_key);
	network_init.dhcp_mode = init_para->dhcp_mode;
	if(init_para->dhcp_mode != DHCP_CLIENT)
	{
		os_strcpy(network_init.local_ip_addr, init_para->local_ip_addr);
		os_strcpy(network_init.net_mask, init_para->net_mask);
		os_strcpy(network_init.gateway_ip_addr, init_para->gateway_ip_addr);
		os_strcpy(network_init.dns_server_ip_addr, init_para->dns_server_ip_addr);
	}

	ret = bk_wlan_start(&network_init);

    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;
	network_InitTypeDef_adv_st wNetConfigAdv;

	os_memset(&wNetConfigAdv, 0x0, sizeof(network_InitTypeDef_adv_st));
	os_strcpy(wNetConfigAdv.ap_info.ssid, init_para_adv->ap_info.ssid);
	os_memcpy(wNetConfigAdv.ap_info.bssid, init_para_adv->ap_info.bssid, 6);
	wNetConfigAdv.ap_info.channel = init_para_adv->ap_info.channel;
	wNetConfigAdv.ap_info.security = init_para_adv->ap_info.security;
	wNetConfigAdv.key_len = init_para_adv->key_len;
	os_memcpy(wNetConfigAdv.key, init_para_adv->key, init_para_adv->key_len);
	wNetConfigAdv.dhcp_mode = init_para_adv->dhcp_mode;
	if(init_para_adv->dhcp_mode != DHCP_CLIENT)
	{
		os_strcpy(wNetConfigAdv.local_ip_addr, init_para_adv->local_ip_addr);
		os_strcpy(wNetConfigAdv.net_mask, init_para_adv->net_mask);
		os_strcpy(wNetConfigAdv.gateway_ip_addr, init_para_adv->gateway_ip_addr);
		os_strcpy(wNetConfigAdv.dns_server_ip_addr, init_para_adv->dns_server_ip_addr);
	}
	
 	ret = bk_wlan_start_sta_adv(&wNetConfigAdv);
	
    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;
	IPStatusTypedef ip_status;

	if(wifi_type == SOFT_AP)
	{
		ret = bk_wlan_get_ip_status(&ip_status, BK_SOFT_AP);
	}
	else
	{
		ret = bk_wlan_get_ip_status(&ip_status, BK_STATION);
	}
	out_net_para->dhcp = ip_status.dhcp;
	os_strcpy(out_net_para->ip, ip_status.ip);
	os_strcpy(out_net_para->gate, ip_status.gate);
	os_strcpy(out_net_para->mask, ip_status.mask);
	os_strcpy(out_net_para->dns, ip_status.dns);
	os_strcpy(out_net_para->mac, ip_status.mac);
	os_strcpy(out_net_para->broadcastip, ip_status.broadcastip);

    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	int ret;
	LinkStatusTypeDef link_status;
	
    ret = bk_wlan_get_link_status(&link_status);

	if(link_status.conn_state == MSG_GOT_IP)
	{
		out_stat->is_connected = 1;
	}
	else
	{
		out_stat->is_connected = 0;
	}
	out_stat->wifi_strength = link_status.wifi_strength;
	os_strcpy(out_stat->ssid, link_status.ssid);
	os_memcpy(out_stat->bssid, link_status.bssid, 6);
	out_stat->channel = link_status.channel;

	return ret;
}

static void scan_cb(void *ctxt, void *user)
{
	struct scanu_rst_upload *scan_rst;
	hal_wifi_scan_result_t apList;
	int i, j;

	apList.ap_list = NULL;
	scan_rst = sr_get_scan_results();
	if (scan_rst == NULL) {
		apList.ap_num = 0;
	} else {
		apList.ap_num = scan_rst->scanu_num;
	}
	if (apList.ap_num > 0) {
		apList.ap_list = os_malloc(sizeof(*apList.ap_list)*apList.ap_num);
		os_memset(apList.ap_list, 0, sizeof(*apList.ap_list)*apList.ap_num);
		for(i=0; i<scan_rst->scanu_num; i++) {
			os_memcpy(apList.ap_list[i].ssid, scan_rst->res[i]->ssid, 32);
			apList.ap_list[i].ap_power = scan_rst->res[i]->level;
		}
	}
	if (apList.ap_list == NULL) {
		apList.ap_num = 0;
	}
	ApListCallback(&apList);
	if (apList.ap_list != NULL) {
		os_free(apList.ap_list);
		apList.ap_list = NULL;
	}
	sr_release_scan_results(scan_rst);
#if CFG_USE_BLE_PS
    rf_can_share_for_ble();
#endif
}

static void scan_adv_cb(void *ctxt, void *user)
{
	struct scanu_rst_upload *scan_rst;
	hal_wifi_scan_result_adv_t apList;
	int i, j;

	apList.ap_list = NULL;
	scan_rst = sr_get_scan_results();
	if (scan_rst == NULL) {
		apList.ap_num = 0;
	} else {
		apList.ap_num = scan_rst->scanu_num;
	}
	if (apList.ap_num > 0) {
		apList.ap_list = os_malloc(sizeof(*apList.ap_list)*apList.ap_num);
		os_memset(apList.ap_list, 0, sizeof(*apList.ap_list)*apList.ap_num);
		for(i=0; i<scan_rst->scanu_num; i++) {
			os_memcpy(apList.ap_list[i].ssid, scan_rst->res[i]->ssid, 32);
			apList.ap_list[i].ap_power = scan_rst->res[i]->level;
			os_memcpy(apList.ap_list[i].bssid, scan_rst->res[i]->bssid, 6);
			apList.ap_list[i].channel = scan_rst->res[i]->channel;
			apList.ap_list[i].security = scan_rst->res[i]->security;
		}
	}
	if (apList.ap_list == NULL) {
		apList.ap_num = 0;
	}
	ApListAdvCallback(&apList);
	if (apList.ap_list != NULL) {
		os_free(apList.ap_list);
		apList.ap_list = NULL;
	}
	sr_release_scan_results(scan_rst);
#if CFG_USE_BLE_PS
    rf_can_share_for_ble();
#endif
}

static void start_scan(hal_wifi_module_t *m)
{
	mhdr_scanu_reg_cb(scan_cb, 0);
    bk_wlan_start_scan();
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	mhdr_scanu_reg_cb(scan_adv_cb, 0);
	bk_wlan_start_scan();
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
    return bk_wlan_suspend();
}

static int suspend_station(hal_wifi_module_t *m)
{
    return bk_wlan_suspend_station();
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return bk_wlan_suspend_softap();
}

static int get_channel(hal_wifi_module_t *m)
{
	return bk_wlan_get_channel();
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return bk_wlan_set_channel(ch);
}

static void start_monitor(hal_wifi_module_t *m)
{
	bk_wlan_start_monitor();
}

static void stop_monitor(hal_wifi_module_t *m)
{
	bk_wlan_stop_monitor();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	bk_wlan_register_monitor_cb(fn);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	bk_wlan_register_mgnt_monitor_cb(fn);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return bk_wlan_send_80211_raw_frame(buf, len); // len-4=exclude FCS
}

void NetCallback(IPStatusTypedef *pnet)
{
	hal_wifi_ip_stat_t net_stat;

	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->ip_got == NULL)
		return;

	net_stat.dhcp = pnet->dhcp;
	os_strcpy(net_stat.ip, pnet->ip);
	os_strcpy(net_stat.mask, pnet->mask);
	os_strcpy(net_stat.gate, pnet->gate);
	os_strcpy(net_stat.dns, pnet->dns);
	os_memcpy(net_stat.mac, pnet->mac, 6);
	os_strcpy(net_stat.broadcastip, pnet->broadcastip);
	
	sim_aos_wifi_beken.ev_cb->ip_got(&sim_aos_wifi_beken, &net_stat, NULL);
}

void connected_ap_info(apinfo_adv_t *ap_info, char *key, int key_len)
{
	hal_wifi_ap_info_adv_t ap_info_adv;
	
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->para_chg == NULL)
		return;

	os_strcpy(ap_info_adv.ssid, ap_info->ssid);
	os_memcpy(ap_info_adv.bssid, ap_info->bssid, 6);
	ap_info_adv.channel = ap_info->channel;
	ap_info_adv.security = ap_info->security;
	sim_aos_wifi_beken.ev_cb->para_chg(&sim_aos_wifi_beken, &ap_info_adv, key, key_len, NULL);
}

void WifiStatusHandler(int status)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->stat_chg == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->stat_chg(&sim_aos_wifi_beken, status, NULL);
}

void ApListCallback(hal_wifi_scan_result_t *pApList)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->scan_compeleted == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->scan_compeleted(&sim_aos_wifi_beken, 
		(hal_wifi_scan_result_t*)pApList, NULL);
}

void ApListAdvCallback(hal_wifi_scan_result_adv_t *pApAdvList)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->scan_adv_compeleted == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->scan_adv_compeleted(&sim_aos_wifi_beken, 
		pApAdvList, NULL);
}

extern void rwnx_go_debug_mode(void);
void start_debug_mode(hal_wifi_module_t *m)
{
    //rwnx_go_debug_mode();
}

void stop_debug_mode(hal_wifi_module_t *m)
{
}

hal_wifi_module_t sim_aos_wifi_beken = {
    .base.name           = "sim_aos_wifi_beken",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .set_mac_addr        =  wifi_set_mac_addr,
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
    .get_channel         =  get_channel,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};

