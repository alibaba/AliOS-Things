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

#ifndef _WIFI_PORT_H_
#define _WIFI_PORT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int xr871_wlan_init(void);
void xr871_wlan_get_mac_addr(uint8_t *mac);
int xr871_wlan_start(hal_wifi_init_type_t *init_para);
int xr871_wlan_start_adv(hal_wifi_init_type_adv_t *init_para_adv);
int xr871_wlan_get_ip_stat(hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);
int xr871_wlan_get_link_stat(hal_wifi_link_stat_t *out_stat);
void xr871_wlan_start_scan(void);
void xr871_wlan_start_adv_scan(void);
int xr871_wlan_power_off(void);
int xr871_wlan_power_on(void);
int xr871_wlan_suspend(void);
int xr871_wlan_suspend_station(void);
int xr871_wlan_suspend_softap(void);
int xr871_wlan_set_channel(int ch);
void xr871_wlan_start_monitor(void);
void xr871_wlan_stop_monitor(void);
void xr871_wlan_register_monitor_cb(monitor_data_cb_t fn);
void xr871_wlan_register_mgnt_monitor_cb(monitor_data_cb_t fn);
int xr871_wlan_send_80211_raw_frame(uint8_t *buf, int len);
void xr871_wlan_start_debug_mode(void);
void xr871_wlan_stop_debug_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* _WIFI_PORT_H_ */
