/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network/hal/base.h"
#include <hal/wifi.h>
#include "common.h"
#include "mico_wlan.h"

hal_wifi_module_t sim_aos_wifi_mico;
static uint16_t _monitor_channel = 1;
static uint8_t  _station_mode_start = 0;
static uint32_t _set_channel_time = 0; // use this VAR to check AWS channel is locked.
#pragma pack(1)
typedef struct
{
    uint8_t type;
    uint8_t flags;
    uint16_t duration;
    uint8_t address1[6];
    uint8_t address2[6];
    uint8_t address3[6];
    uint16_t seq;
    uint8_t data[1];
} ieee80211_header_t;
#pragma pack()

static int custom_ie_added = 0;
static void mico_wlan_monitor_cb(uint8_t*data, int len);

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
    _station_mode_start = 1;
    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;

 	ret = micoWlanStartAdv((network_InitTypeDef_adv_st*)init_para_adv);
	_station_mode_start = 1;
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
    _station_mode_start = 0;
    return micoWlanSuspend();
}

static int suspend_station(hal_wifi_module_t *m)
{
    _station_mode_start = 0;
    return micoWlanSuspendStation();
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return micoWlanSuspendSoftAP();
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    int ret;
    
    if (ch == _monitor_channel) {
        return 0;
    }
    _monitor_channel = ch;
    _set_channel_time = aos_now_ms();

    ret = mico_wlan_monitor_set_channel(ch);
    if (ret != 0) {
        printf("set channel %d error %d\r\n", ch, ret);
        wifi_reboot_only();
        mico_wlan_register_monitor_cb(mico_wlan_monitor_cb);
        mico_wlan_start_monitor();
        custom_ie_added = 0;
    }
    return ret;
}

static void start_monitor(hal_wifi_module_t *m)
{
    if (_station_mode_start == 1) {
        suspend_station(m); // softap mode -> monitor mode, must do suspend station. 
    }
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

typedef struct
{
    uint8_t len;     /**< SSID length */
    uint8_t val[32]; /**< SSID name (AP name)  */
} wiced_ssid_t;
typedef struct
{
    int32_t number_of_probes_per_channel;                     /**< Number of probes to send on each channel                                               */
    int32_t scan_active_dwell_time_per_channel_ms;            /**< Period of time to wait on each channel when active scanning                            */
    int32_t scan_passive_dwell_time_per_channel_ms;           /**< Period of time to wait on each channel when passive scanning                           */
    int32_t scan_home_channel_dwell_time_between_channels_ms; /**< Period of time to wait on the home channel when scanning. Only relevant if associated. */
} wiced_scan_extended_params_t;

static void scan_results_handler( void * result, void *arg2 )
{
   
}


static /*@null@*/ uint8_t* wlu_parse_tlvs( /*@returned@*/ uint8_t* tlv_buf, int buflen, uint8_t key )
{
    uint8_t* cp = tlv_buf;
    int  totlen = buflen;

    /* find tagged parameter */
    while ( totlen >= 2 )
    {
        uint8_t tag;
        uint8_t len;

        tag = cp[0];
        len = cp[1];

        /* validate remaining totlen */
        if ( ( tag == key ) && ( totlen >= ( len + 2 ) ) )
        {
            return ( cp );
        }

        cp += ( len + 2 );
        totlen -= ( len + 2 );
    }

    return NULL;
}

static const uint8_t oui[] = {0xD8,  0x96,  0xE0};

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    ieee80211_header_t *header = (ieee80211_header_t*)buf;
    uint8_t *parse, *ie, *ali_ie;
    int     parse_len;
    wiced_ssid_t ssid;
    uint16_t chlist[2];
    wiced_scan_extended_params_t extparam = { 2, 50, 50, 50 };

    if (_set_channel_time+1000 < aos_now_ms()) { // channel locked, don't send probe requests
        return 0;
    }
    if (header->type != 0x40) {
        return 0;
    }
    len -= 24;
    parse = (uint8_t*)header + 24; // IE buffer header
    parse_len = len;

    ssid.len = 0;
    if ( ( ie = wlu_parse_tlvs( parse, parse_len, (uint8_t) 0 ) ) != 0 )
    {
    	if (ie[1] > 0) {
            ssid.len = ie[1];
            memcpy(ssid.val, &ie[2], ie[1]);
        }
    }
    if (custom_ie_added == 0) {
        if ( ( ie = wlu_parse_tlvs( parse, parse_len, 0xDD ) ) != 0 )
        {
        	if (ie[2] == oui[0] && ie[3] == oui[1] && ie[4] == oui[2]) {
                printf("Add custom IE (%d)\r\n", ie[1]);
                wiced_wifi_manage_custom_ie(0, 0, &ie[2], ie[5], 
                    &ie[6], ie[1] - 4, 0x10);
                custom_ie_added = 1;
            }
        }
    }
    chlist[0] = _monitor_channel;
    chlist[1] = 0;

    if (ssid.len > 0) { 
        ssid.val[ssid.len] = 0;
        wiced_wifi_scan(0, 2, &ssid, NULL, chlist, &extparam, 
            scan_results_handler, NULL, NULL);
        aos_msleep(60); // wait scan done
    }

    return kNoErr;
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

static int get_wireless_info(hal_wifi_module_t *m, hal_wireless_info_t *wireless_info)
{
    int ret = -1;
    LinkStatusTypeDef status = {0};

    if (wireless_info) {
        ret = micoWlanGetLinkStatus(&status);
        if (ret == 0) {
            if (status.rssi > 127) status.rssi = 127;
            if (status.rssi == 0) status.rssi = -1;
            if (status.rssi > 0) status.rssi -= 128;
            wireless_info->rssi = status.rssi;
        }
    }

    return ret;
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
    .get_wireless_info   =  get_wireless_info,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};

