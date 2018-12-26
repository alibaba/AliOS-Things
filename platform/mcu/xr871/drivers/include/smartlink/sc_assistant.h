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

#ifndef _SC_ASSISTANT_H_
#define _SC_ASSISTANT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "net/wlan/wlan.h"
#include "lwip/netif.h"

#define MONITOR_MAX_CB  4 /* max num of rx callback support */

typedef enum {
	SCA_STATUS_END = 0,     /* also init status */
	SCA_STATUS_CONTINUE,
	SCA_STATUS_CHANNEL_LOCKED,
	SCA_STATUS_COMPLETE,
	SCA_STATUS_CRC_ERROR,
} sc_assistant_status;

typedef uint8_t *(*sca_get_mac)(uint8_t *mac_hex);
typedef void (*sca_open_monitor)(void);
typedef void (*sca_close_monitor)(void);
typedef void (*sca_switch_channel)(char channel);
typedef int (*sca_send_raw_frame)(int type, uint8_t *buffer, int len);
typedef struct netif *(*sca_open_sta)(void);
typedef int32_t (*sca_connect_ap)(uint8_t *ssid, int ssid_len, uint8_t *psk, unsigned int timeout_ms);
typedef int32_t (*sca_get_ip)(char *ip_str, const char *ifname);

typedef struct
{
	sca_get_mac get_mac;
	sca_open_monitor open_monitor;
	sca_close_monitor close_monitor;
	sca_switch_channel switch_channel;
	sca_open_sta open_sta;
	sca_connect_ap connect_ap;
	void (*stop_connect_ap)(void);
	sca_get_ip get_ip;
	sca_send_raw_frame send_raw_frame;
} sc_assistant_fun_t;

/**
 * @brief Parameter for configurate sc_assistant time
 * Note:time_total means the total time for sc_assistant timeout;
 *         time_sw_ch_long means max time stay in each channel;
 *         time_sw_ch_short means min time stay in each channel;
 *         Set both time_sw_ch* to 0 means do not change channel by sc_assistant;
 *         We should not set only one of time_sw_ch* to 0, that makes an error return.
 */
typedef struct
{
	uint32_t time_total;
	uint16_t time_sw_ch_long;
	uint16_t time_sw_ch_short;
} sc_assistant_time_config_t;

int sc_assistant_monitor_register_rx_cb(struct netif *nif, wlan_monitor_rx_cb cb);
int sc_assistant_monitor_unregister_rx_cb(struct netif *nif, wlan_monitor_rx_cb cb);
typedef void (*sc_assistant_monitor_sw_ch_cb)(struct netif *nif, int16_t channel);
int sc_assistant_monitor_register_sw_ch_cb(struct netif *nif,
                                           sc_assistant_monitor_sw_ch_cb cb);
int sc_assistant_monitor_unregister_sw_ch_cb(struct netif *nif, sc_assistant_monitor_sw_ch_cb cb);
uint8_t *sc_assistant_get_mac(uint8_t *mac_hex);
void sc_assistant_open_monitor(void);
void sc_assistant_close_monitor(void);
void sc_assistant_switch_channel(char channel);
int sc_assistant_send_raw_frame(int type, uint8_t *buffer, int len);
struct netif *sc_assistant_open_sta(void);
int sc_assistant_connect_ap(uint8_t *ssid, int ssid_len, uint8_t *psk, unsigned int timeout_ms);
/* stop connecting ap if sc_assistant is connecting ap */
void sc_assistant_stop_connect_ap(void);
int32_t sc_assistant_get_ip(char *ip_str, const char *ifname);
void sc_assistant_get_fun(sc_assistant_fun_t *fun);
sc_assistant_status sc_assistant_get_status(void);
uint32_t sc_assistant_get_sw_ch_interval_ms(void);
int16_t sc_assistant_get_ap_channel(void);
extern void sc_assistant_newstatus(sc_assistant_status status, uint8_t *ap_mac, void *info);
int sc_assistant_init(struct netif *nif, sc_assistant_fun_t *cb, sc_assistant_time_config_t *config);
int sc_assistant_deinit(struct netif *nif);

#ifdef __cplusplus
}
#endif

#endif /* _SC_ASSISTANT_H_ */
