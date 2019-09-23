<<<<<<< HEAD
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network/hal/base.h"
#include <hal/wifi.h>

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

	ret = bk_wlan_start(init_para);

    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;

 	ret = bk_wlan_start_sta_adv(init_para_adv);
	
    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;

	ret = bk_wlan_get_ip_status(out_net_para, wifi_type);

    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	int ret;
	
    ret = bk_wlan_get_link_status(out_stat);

	return ret;
}

static void start_scan(hal_wifi_module_t *m)
{
    bk_wlan_start_scan(0);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	bk_wlan_start_scan(1);
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
    return bk_wlan_send_80211_raw_frame(buf, len - 4); // len-4=exclude FCS
}

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->ip_got == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->ip_got(&sim_aos_wifi_beken, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->para_chg == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->para_chg(&sim_aos_wifi_beken, ap_info, key, key_len, NULL);
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
	int i;
	
	printf("AP %d: \r\n", pApList->ap_num);
	for(i=0; i<pApList->ap_num; i++) {
		printf("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
	}
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
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};

=======
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>

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

	ret = bk_wlan_start(init_para);

    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;

 	ret = bk_wlan_start_sta_adv(init_para_adv);
	
    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;

	ret = bk_wlan_get_ip_status(out_net_para, wifi_type);

    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	int ret;
	
    ret = bk_wlan_get_link_status(out_stat);

	return ret;
}

static void start_scan(hal_wifi_module_t *m)
{
    bk_wlan_start_scan(0);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	bk_wlan_start_scan(1);
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

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->ip_got == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->ip_got(&sim_aos_wifi_beken, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
	if (sim_aos_wifi_beken.ev_cb == NULL)
		return;
	if (sim_aos_wifi_beken.ev_cb->para_chg == NULL)
		return;

	sim_aos_wifi_beken.ev_cb->para_chg(&sim_aos_wifi_beken, ap_info, key, key_len, NULL);
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
	int i;
	
	printf("AP %d: \r\n", pApList->ap_num);
	for(i=0; i<pApList->ap_num; i++) {
		printf("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
	}
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

>>>>>>> 62b1b7599 (BugID:23044503: Modify format of uND packet & reporting scheme of uND.)
