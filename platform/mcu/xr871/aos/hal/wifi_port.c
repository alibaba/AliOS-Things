/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <lwip/inet.h>
#include "common.h"
#include "net/wlan/wlan.h"
#include "net_ctrl.h"
#include "wifi_port.h"
#include "sysinfo.h"
#include "kernel/os/os.h"
#include "common/framework/sys_ctrl/sys_ctrl.h"

#define WIFI_DEBUG(...) do { \
                            printf("[wifi_port]: "); \
                            printf(__VA_ARGS__); \
                        } while (0)

#define MAX_SCAN_RESULTS	10

extern struct netif *g_wlan_netif;

static wlan_monitor_rx_cb g_monitor_cb;
static int g_network_up = 0;

static enum scan_type {
    SCAN_TYPE_NORMAL,
    SCAN_TYPE_ADVANCE
} g_scan_type = SCAN_TYPE_NORMAL;

void xr871_wlan_msg_process(uint32_t event, uint32_t data)
{
	struct netif *nif = g_wlan_netif;
	uint16_t type = EVENT_SUBTYPE(event);

	WIFI_DEBUG("%s msg (%u, %u)\n", __func__, type, data);
	switch (type) {
	case NET_CTRL_MSG_WLAN_CONNECTED:
		break;
	case NET_CTRL_MSG_WLAN_SCAN_SUCCESS:
		if (g_scan_type == SCAN_TYPE_NORMAL)
			xr871_scan_compeleted_event();
		if (g_scan_type == SCAN_TYPE_ADVANCE)
			xr871_scan_adv_compeleted_event();
		break;
	case NET_CTRL_MSG_WLAN_CONNECT_FAILED:
		xr871_connect_fail_event();
		break;
	case NET_CTRL_MSG_NETWORK_UP:
		if (wlan_if_get_mode(nif) == WLAN_MODE_STA) {
			xr871_ip_got_event();
		}
		g_network_up = 1;
		xr871_stat_chg_event();
		break;
	case NET_CTRL_MSG_NETWORK_DOWN:
		g_network_up = 0;
		xr871_stat_chg_event();
		break;
	case NET_CTRL_MSG_WLAN_DISCONNECTED:
	case NET_CTRL_MSG_WLAN_SCAN_FAILED:
	case NET_CTRL_MSG_WLAN_4WAY_HANDSHAKE_FAILED:
		WIFI_DEBUG("do nothing msg (%u, %u)\n", type, data);
		break;
	default:
		WIFI_DEBUG("unknown msg (%u, %u)\n", type, data);
		break;
	}
}

int xr871_wlan_init(void)
{
	// create network event listener
	observer_base *ob;

	WIFI_DEBUG("wlan init, register network observer\n");
	ob = sys_callback_observer_create(CTRL_MSG_TYPE_NETWORK,
									  NET_CTRL_MSG_ALL,
									  xr871_wlan_msg_process);
	if (ob == NULL)
		return -1;
	if (sys_ctrl_attach(ob) != 0)
		return -1;

	WIFI_DEBUG("xr871 wifi init success!!\n");
	return 0;
}

void xr871_wlan_get_mac_addr(uint8_t *mac)
{
	struct sysinfo *info = sysinfo_get();;

	WIFI_DEBUG("wlan get mac address\n");
	memcpy(mac, info->mac_addr, 6);
	WIFI_DEBUG("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int xr871_wlan_start(hal_wifi_init_type_t *init_para)
{
	int ret = 0;
	int ssid_len = strlen(init_para->wifi_ssid);
	int psk_len = strlen(init_para->wifi_key);

	WIFI_DEBUG("wlan start, mode %d\n", init_para->wifi_mode);

	if (ssid_len > 32)
		ssid_len = 32;
	switch (init_para->wifi_mode) {
	case STATION: /* STA */
		WIFI_DEBUG("station start, ssid %s, key %s\n",
					init_para->wifi_ssid, init_para->wifi_key);
		net_switch_mode(WLAN_MODE_STA);
		wlan_sta_disable();
		if (psk_len > 0) {
			wlan_sta_set(init_para->wifi_ssid, ssid_len, init_para->wifi_key);
		}
		else {
			wlan_sta_set(init_para->wifi_ssid, ssid_len, NULL);
		}
		wlan_sta_enable();
		break;

	case SOFT_AP: /* AP */
		WIFI_DEBUG("hostap start, ssid %s, key %s\n",
					init_para->wifi_ssid, init_para->wifi_key);
		net_switch_mode(WLAN_MODE_HOSTAP);
		wlan_ap_disable();
		wlan_ap_set(init_para->wifi_ssid, ssid_len, init_para->wifi_key);
		wlan_ap_enable();
		break;

	default:
		WIFI_DEBUG("error wifi mode\n");
		break;
	}

	return ret;
}

int xr871_wlan_start_adv(hal_wifi_init_type_adv_t *init_para_adv)
{
	int ssid_len = strlen(init_para_adv->ap_info.ssid);
	int psk_len = strlen(init_para_adv->key);
	WIFI_DEBUG("wlan start advance\n");
	if (ssid_len > 32)
		ssid_len = 32;
	net_switch_mode(WLAN_MODE_STA);
	wlan_sta_disable();
	if (psk_len > 0) {
		wlan_sta_set(init_para_adv->ap_info.ssid, ssid_len, init_para_adv->key);
	}
	else {
		wlan_sta_set(init_para_adv->ap_info.ssid, ssid_len, NULL);
	}
	wlan_sta_enable();

	return 0;
}

int xr871_wlan_get_ip_stat(hal_wifi_ip_stat_t *out_net_para,
							hal_wifi_type_t wifi_type)
{
	struct netif *nif = g_wlan_netif;

	if (nif == NULL || out_net_para == NULL) {
		return -1;
	}

	xr871_wlan_get_mac_addr(out_net_para->mac);	                                   /* mac */
	if (netif_is_up(nif) && netif_is_link_up(nif)) {
		inet_ntoa_r(nif->ip_addr, out_net_para->ip, sizeof(out_net_para->ip));     /* ip */
		inet_ntoa_r(nif->gw, out_net_para->gate, sizeof(out_net_para->gate));      /* gate */
		inet_ntoa_r(nif->netmask, out_net_para->mask, sizeof(out_net_para->mask)); /* mask */
		WIFI_DEBUG("%c%c%d up, address:%s gateway:%s netmask:%s",
						  nif->name[0], nif->name[1], nif->num,
						  out_net_para->ip, out_net_para->gate, out_net_para->mask);
	} else {
		WIFI_DEBUG("%c%c%d down", nif->name[0], nif->name[1], nif->num);
		return -1;
	}

	/* dhcp */
	/* dns */
	/* broadcastip */
	if (wifi_type == STATION) {

	} else if (wifi_type == SOFT_AP) {

	}

	return 0;
}

int xr871_wlan_get_link_stat(hal_wifi_link_stat_t *out_stat)
{
	wlan_sta_states_t state;
	wlan_sta_ap_t ap;

	WIFI_DEBUG("wlan get link stat\n");
	if (out_stat == NULL) {
		return -1;
	}
	memset(out_stat, 0, sizeof(hal_wifi_link_stat_t));

	/* is_connected */
	wlan_sta_state(&state);
	if (state == WLAN_STA_STATE_CONNECTED) {
		wlan_sta_ap_info(&ap);
		out_stat->is_connected = 1;
		out_stat->wifi_strength = ap.rssi;
		out_stat->channel = ap.channel;
		memcpy(out_stat->ssid, ap.ssid.ssid, sizeof(ap.ssid.ssid_len));
		memcpy(out_stat->bssid, ap.bssid, sizeof(out_stat->bssid));

		WIFI_DEBUG("======= link state =======\n");
		WIFI_DEBUG("connect   :\n", out_stat->is_connected);
		WIFI_DEBUG("strength  :\n", out_stat->wifi_strength);
		WIFI_DEBUG("ssid      :%-32.32s\n", out_stat->ssid);
		WIFI_DEBUG("bssid     %02x:%02x:%02x:%02x:%02x:%02x\n",
					out_stat->bssid[0], out_stat->bssid[1], out_stat->bssid[2],
					out_stat->bssid[3], out_stat->bssid[4], out_stat->bssid[5]);
		WIFI_DEBUG("channel   :%d\n", out_stat->channel);

	} else {
		WIFI_DEBUG("Request link state in HostAP mode ???\n");
		out_stat->is_connected = 0;
	}

	return 0;
}

void xr871_wlan_start_scan(void)
{
	WIFI_DEBUG("wlan start normal scan\n");
	g_scan_type = SCAN_TYPE_NORMAL;

	wlan_sta_scan_once();
}

void xr871_wlan_start_adv_scan(void)
{
	WIFI_DEBUG("wlan start advance scan\n");
	g_scan_type = SCAN_TYPE_ADVANCE;

	wlan_sta_scan_once();
}

int xr871_wlan_power_off(void)
{
	WIFI_DEBUG("wlan power off\n");
	net_sys_stop();
	return 0;
}

int xr871_wlan_power_on(void)
{
	WIFI_DEBUG("wlan power on\n");
	net_sys_start(WLAN_MODE_STA);
	return 0;
}

int xr871_wlan_suspend(void)
{
	struct netif *nif = g_wlan_netif;

	WIFI_DEBUG("wlan suspend\n");
	if (nif != NULL) {
		enum wlan_mode mode = wlan_if_get_mode(nif);
		if (mode == WLAN_MODE_HOSTAP) {
			wlan_ap_disable();
		}
		if (mode == WLAN_MODE_STA) {
			wlan_sta_disable();
		}
		if (mode == WLAN_MODE_MONITOR) {
		}
	}
	return 0;
}

int xr871_wlan_suspend_station(void)
{
	WIFI_DEBUG("wlan station suspend\n");
	wlan_sta_disable();
	return 0;
}

int xr871_wlan_suspend_softap(void)
{
	WIFI_DEBUG("wlan softap suspend\n");
	wlan_ap_disable();
	return 0;
}

int xr871_wlan_set_channel(int ch)
{
	struct netif *nif = g_wlan_netif;

	WIFI_DEBUG("wlan set monitor channel %d\n", ch);
	return wlan_monitor_set_channel(nif, ch);
}

void xr871_wlan_start_monitor(void)
{
	struct netif *nif = g_wlan_netif;

	WIFI_DEBUG("wlan start monitor mode, nif %p, cb %p\n", nif, g_monitor_cb);

	if (nif) {
		wlan_monitor_set_rx_cb(nif, g_monitor_cb);
		net_switch_mode(WLAN_MODE_MONITOR);
	}
}

void xr871_wlan_stop_monitor(void)
{
	struct netif *nif = g_wlan_netif;

	WIFI_DEBUG("wlan stop monitor mode\n");
	if (nif) {
		wlan_monitor_set_rx_cb(nif, NULL);
	}
}

void xr871_wlan_register_monitor_cb(monitor_data_cb_t fn)
{
	WIFI_DEBUG("wlan register monitor callback: %p\n", fn);
	g_monitor_cb = (wlan_monitor_rx_cb)fn;
}

void xr871_wlan_register_mgnt_monitor_cb(monitor_data_cb_t fn)
{
	WIFI_DEBUG("wlan register manage monitor callback: %p\n", fn);
	g_monitor_cb = (wlan_monitor_rx_cb)fn;
}

int xr871_wlan_send_80211_raw_frame(uint8_t *buf, int len)
{
	WIFI_DEBUG("send raw frame(not support now)\n");

	// dump raw frame data
	if (0)
	{
		uint32_t i = 0;
		printf("dump frame data:\n");
		for (i = 0; i < len; i++) {
			printf("%02x ", buf[i]);
			if ((i % 32) == 31)
				printf("\n");
		}
		printf("\n");
    }
	return 0;
}

void xr871_wlan_start_debug_mode(void)
{
	WIFI_DEBUG("start wifi debug mode\n");
}

void xr871_wlan_stop_debug_mode(void)
{
	WIFI_DEBUG("stop wifi debug mode\n");
}

void xr871_connect_fail_event(int err, void *arg)
{
	hal_wifi_module_t *m = hal_wifi_get_default_module();

	WIFI_DEBUG("handle connect fail event\n");

	if (m != NULL && m->ev_cb != NULL && m->ev_cb->connect_fail != NULL) {
		m->ev_cb->connect_fail(m, err, arg);
	}
}

void xr871_ip_got_event(void)
{
	hal_wifi_module_t *m = hal_wifi_get_default_module();

	WIFI_DEBUG("handle ip got event\n");
	if (m != NULL && m->ev_cb != NULL && m->ev_cb->ip_got != NULL) {
		hal_wifi_ip_stat_t net = {0};
		xr871_wlan_get_ip_stat(&net, STATION);
		m->ev_cb->ip_got(m, &net, NULL);
	}
}

void xr871_stat_chg_event(void)
{
	hal_wifi_module_t *m = hal_wifi_get_default_module();
	struct netif *nif = g_wlan_netif;

	WIFI_DEBUG("handle state change event\n");

	if (nif != NULL && m != NULL && m->ev_cb != NULL && m->ev_cb->stat_chg != NULL) {
		hal_wifi_event_t stat;
		enum wlan_mode mode = wlan_if_get_mode(nif);
		if (mode == WLAN_MODE_STA) {
			stat = netif_is_link_up(nif) ? NOTIFY_STATION_UP : NOTIFY_STATION_DOWN;
		} else if (mode == WLAN_MODE_HOSTAP) {
			stat = netif_is_link_up(nif) ? NOTIFY_AP_UP : NOTIFY_AP_DOWN;
		}
		m->ev_cb->stat_chg(m, stat, NULL);
	}
}

static const uint32_t channel_freq_tbl[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462, 2467, 2472
};

static int32_t freq_to_chan(int32_t freq)
{
	int32_t i;
	uint32_t chs = sizeof(channel_freq_tbl)/sizeof(channel_freq_tbl[0]);

	for (i = 0; i < chs && freq != channel_freq_tbl[i]; i++);
	if(i == chs) {
		return -1;
	}

	return i + 1;
}

static char rssi_to_ap_power(int32_t rssi)
{
	char ap_power = rssi * 100L / 128;

	if (ap_power > 100) {
		ap_power = 100;
	}

	return ap_power;
}

void xr871_scan_compeleted_event(enum scan_type type)
{
	hal_wifi_module_t *m = hal_wifi_get_default_module();
	hal_wifi_scan_result_t result = {0};
	wlan_sta_scan_results_t scan_results = {0};
	int ret;

	scan_results.ap = malloc(MAX_SCAN_RESULTS * sizeof(wlan_sta_ap_t));
	if (scan_results.ap != NULL) {
		scan_results.size = MAX_SCAN_RESULTS;
		ret = wlan_sta_scan_result(&scan_results);
		if (ret == 0) {
			result.ap_num = scan_results.num;
			result.ap_list = malloc(result.ap_num * sizeof(*(result.ap_list)));

			WIFI_DEBUG("scan result: ap_num: %d, ap %p, ap_list %p\n", result.ap_num,
							scan_results.ap, result.ap_list);

			if (result.ap_list != NULL) {
				int i;

				for (i = 0; i < scan_results.num; i++) {
					memset(result.ap_list[i].ssid, 0, sizeof(result.ap_list[i].ssid));
					memcpy(result.ap_list[i].ssid, scan_results.ap[i].ssid.ssid,
													scan_results.ap[i].ssid.ssid_len);
					result.ap_list[i].ap_power = rssi_to_ap_power(scan_results.ap[i].rssi);

					WIFI_DEBUG("#%-2d: ssid: %-32.32s ap_power: %d\n", i+1,
								result.ap_list[i].ssid, result.ap_list[i].ap_power);
				}

				if (m->ev_cb != NULL && m->ev_cb->scan_compeleted != NULL) {
					m->ev_cb->scan_compeleted(m, &result, NULL);
				}

				free(result.ap_list);
			}
		}

		free(scan_results.ap);
	}
}

void xr871_scan_adv_compeleted_event(enum scan_type type)
{
	hal_wifi_module_t *m = hal_wifi_get_default_module();
	hal_wifi_scan_result_adv_t result = {0};
	wlan_sta_scan_results_t scan_results = {0};
	wlan_sta_config_t config = {0};
	int kmgt, cipher;
	int ret;

	scan_results.ap = malloc(MAX_SCAN_RESULTS * sizeof(wlan_sta_ap_t));
	if (scan_results.ap != NULL) {
		scan_results.size = MAX_SCAN_RESULTS;
		ret = wlan_sta_scan_result(&scan_results);
		if (ret == 0) {
			result.ap_num = scan_results.num;
			result.ap_list = malloc(result.ap_num * sizeof(*(result.ap_list)));

			WIFI_DEBUG("scan result: ap_num: %d\n", result.ap_num);

			if (result.ap_list != NULL) {
				int i;

				for (i = 0; i < scan_results.num; i++) {
					result.ap_list[i].ap_power = rssi_to_ap_power(scan_results.ap[i].rssi);
					result.ap_list[i].channel = freq_to_chan(scan_results.ap[i].freq);
					memset(result.ap_list[i].ssid, 0, sizeof(result.ap_list[i].ssid));
					memcpy(result.ap_list[i].ssid, scan_results.ap[i].ssid.ssid,
													scan_results.ap[i].ssid.ssid_len);
					memcpy(result.ap_list[i].bssid, scan_results.ap[i].bssid,
													sizeof(result.ap_list[i].bssid));

					config.field = WLAN_STA_FIELD_KEY_MGMT;
					if (wlan_sta_get_config(&config) != 0) {
						WIFI_DEBUG("***error, get config failed\n");
					}
					if (config.u.key_mgmt = WPA_KEY_MGMT_NONE) {
						result.ap_list[i].security = SECURITY_TYPE_NONE;
					} else {
						// Fixup:
						WIFI_DEBUG("***warning, temp security impl\n");
						result.ap_list[i].security = SECURITY_TYPE_WPA2_MIXED;
					}

					WIFI_DEBUG("#%-2d: ssid: %-32.32s ap_power: %d, ch %d, security %d "
								"bssid: %02x:%02x:%02x:%02x:%02x:%02x",
								i+1,
								result.ap_list[i].ssid, result.ap_list[i].ap_power,
								result.ap_list[i].channel, result.ap_list[i].security,
								result.ap_list[i].bssid[0], result.ap_list[i].bssid[1],
								result.ap_list[i].bssid[2], result.ap_list[i].bssid[3],
								result.ap_list[i].bssid[4], result.ap_list[i].bssid[5]);
				}

				if (m->ev_cb != NULL && m->ev_cb->scan_compeleted != NULL) {
					m->ev_cb->scan_compeleted(m, &result, NULL);
				}

				free(result.ap_list);
			}
		}

		free(scan_results.ap);
	}
}

void xr871_para_chg_event(void)
{
	WIFI_DEBUG("%s To Do\n", __func__);
}

void xr871_fatal_err_event(void *arg)
{
	WIFI_DEBUG("%s To Do\n", __func__);
}

