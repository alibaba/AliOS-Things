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

#ifndef _NET_WLAN_WLAN_H_
#define _NET_WLAN_WLAN_H_

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE))

#include <stdint.h>
#include "lwip/netif.h"
#include "sys/ducc/ducc_net.h"
#include "sys/ducc/ducc_app.h"
#include "net/wlan/wlan_defs.h"
#include "net/wlan/ethernetif.h"
#include "net/wlan/wlan_smart_config.h"
#include "net/wlan/wlan_airkiss.h"

#ifdef __cplusplus
extern "C" {
#endif

/* wlan sys */
int wlan_sys_init(enum wlan_mode mode, ducc_cb_func cb);
int wlan_sys_deinit(void);

int wlan_start(struct netif *nif);
int wlan_stop(void); /* Note: make sure wlan is disconnect before calling wlan_stop() */

static __inline int wlan_attach(void)
{
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_ATTACH, NULL);
}

static __inline int wlan_detach(void)
{
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_DETACH, NULL);
}

static __inline struct netif *wlan_if_create(enum wlan_mode mode)
{
	return ethernetif_create(mode);
}

static __inline int wlan_if_delete(struct netif *nif)
{
	ethernetif_delete(nif);
	return 0;
}

static __inline enum wlan_mode wlan_if_get_mode(struct netif *nif)
{
	return ethernetif_get_mode(nif);
}

int wlan_set_mac_addr(uint8_t *mac_addr, int mac_len);
int wlan_set_ip_addr(struct netif *nif, uint8_t *ip_addr, int ip_len);
int wlan_set_appie(struct netif *nif, uint8_t type, uint8_t *ie, uint16_t ie_len);

/* STA */
int wlan_sta_set(uint8_t *ssid, uint8_t ssid_len, uint8_t *psk);
int wlan_sta_set_config(wlan_sta_config_t *config);
int wlan_sta_get_config(wlan_sta_config_t *config);

int wlan_sta_enable(void);
int wlan_sta_disable(void);

int wlan_sta_scan_once(void);
int wlan_sta_scan_result(wlan_sta_scan_results_t *results);
int wlan_sta_scan_interval(int sec);
int wlan_sta_bss_flush(int age);

int wlan_sta_connect(void);
int wlan_sta_disconnect(void);

int wlan_sta_state(wlan_sta_states_t *state);
int wlan_sta_ap_info(wlan_sta_ap_t *ap);

int wlan_sta_wps_pbc(void);
int wlan_sta_wps_pin_get(wlan_sta_wps_pin_t *wps);
int wlan_sta_wps_pin_set(wlan_sta_wps_pin_t *wps);

/* softAP */
int wlan_ap_set(uint8_t *ssid, uint8_t ssid_len, uint8_t *psk);
int wlan_ap_set_config(wlan_ap_config_t *config);
int wlan_ap_get_config(wlan_ap_config_t *config);

int wlan_ap_enable(void);
int wlan_ap_reload(void);
int wlan_ap_disable(void);

int wlan_ap_sta_num(int *num);
int wlan_ap_sta_info(wlan_ap_stas_t *stas);

/* monitor */
typedef void (*wlan_monitor_rx_cb)(uint8_t *data, uint32_t len, void *info);
int wlan_monitor_set_rx_cb(struct netif *nif, wlan_monitor_rx_cb cb);
int wlan_monitor_set_channel(struct netif *nif, int16_t channel);
void wlan_monitor_input(struct netif *nif, uint8_t *data, uint32_t len, void *info);

#ifdef __cplusplus
}
#endif

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */

#endif /* _NET_WLAN_WLAN_H_ */
