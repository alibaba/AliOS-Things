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
#include <network/hal/base.h>
#include <network/hal/wifi.h>
#include <net/wlan/wlan_defs.h>
#include "wifi_port.h"


static int wifi_init(hal_wifi_module_t *m)
{
	return xr871_wlan_init();
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
	xr871_wlan_get_mac_addr(mac);
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    int ret;

	ret = xr871_wlan_start(init_para);

    return ret;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret;

 	ret = xr871_wlan_start_adv(init_para_adv);

    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;

	ret = xr871_wlan_get_ip_stat(out_net_para, wifi_type);

    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	int ret;

    ret = xr871_wlan_get_link_stat(out_stat);

	return ret;
}

static void start_scan(hal_wifi_module_t *m)
{
    xr871_wlan_start_scan();
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	xr871_wlan_start_adv_scan();
}

static int power_off(hal_wifi_module_t *m)
{
   	return xr871_wlan_power_off();
}

static int power_on(hal_wifi_module_t *m)
{
    return xr871_wlan_power_on();
}

static int suspend(hal_wifi_module_t *m)
{
    return xr871_wlan_suspend();
}

static int suspend_station(hal_wifi_module_t *m)
{
    return xr871_wlan_suspend_station();
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return xr871_wlan_suspend_softap();
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return xr871_wlan_set_channel(ch);
}

static void start_monitor(hal_wifi_module_t *m)
{
	xr871_wlan_start_monitor();
}

static void stop_monitor(hal_wifi_module_t *m)
{
	xr871_wlan_stop_monitor();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	xr871_wlan_register_monitor_cb(fn);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    xr871_wlan_register_mgnt_monitor_cb(fn);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return xr871_wlan_send_80211_raw_frame(buf, len - 4); // len-4=exclude FCS
}

static void start_debug_mode(hal_wifi_module_t *m)
{
	xr871_wlan_start_debug_mode();
}

static void stop_debug_mode(hal_wifi_module_t *m)
{
	xr871_wlan_stop_debug_mode();
}

static int get_wireless_info(hal_wifi_module_t *m, void *wireless_info)
{
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;
	wlan_sta_states_t state;
	wlan_sta_ap_t ap;

    printf("get wireless info\r\n");

    if (info == NULL)
        return -1;

	wlan_sta_state(&state);
	if (state == WLAN_STA_STATE_CONNECTED) {
		wlan_sta_ap_info(&ap);
        info->rssi = ap.rssi;
        return 0;
    }

    return -1;
}

hal_wifi_module_t sim_aos_wifi_xr871 = {
    .base.name           = "sim_aos_wifi_xr871",
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
    .stop_debug_mode = stop_debug_mode,
    .get_wireless_info   = get_wireless_info,
};

