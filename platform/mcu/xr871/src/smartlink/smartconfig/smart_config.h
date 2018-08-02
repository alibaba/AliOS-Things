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

#ifndef _SMART_CONFIG__H_
#define _SMART_CONFIG__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "net/wlan/wlan_defs.h"
#include "smartlink/smart_config/wlan_smart_config.h"

/*
 * define SMARTCONFIG_ENABLE_CRYPT 1 to use smartconfig encryption.
 */
#ifndef SMARTCONFIG_ENABLE_CRYPT
#define SMARTCONFIG_ENABLE_CRYPT 1
#endif

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define SMART_DBG(level, fmt, args...)                  \
	do {                                            \
		if (level <= g_debuglevel)              \
			printf("[SC]"fmt,##args);       \
	} while (0)

#define SC_DBG SMART_DBG

#define SC_BUG_ON(v) do {if(v) {printf("BUG at %s:%d!\n", __func__, __LINE__); \
                         while (1);}} while (0)

#define SC_KEY_LEN 16
#define MAX_TIME 4294967295
#define SC_ACK_TIME_OUT 30000
#define SC_ACK_UDP_PORT 10000
#define SC_MAX_SSIDPSK_LEN 128  /* the max length of psk and ssid after AES encryption */

typedef struct {
	int channel;
	int ssidpwd_len;
	int pwd_len;
	int packet_total;
	int packet_count;
	uint8_t random;
	int locked_mac_flag;
	uint8_t lead_complete_flag;
	//uint8_t locked_mac[6];
	uint8_t count_pkt[16];
} sc_lead_code_t;

typedef struct {
	uint8_t ssid[WLAN_SSID_MAX_LEN];
	uint8_t pwd[WLAN_PASSPHRASE_MAX_LEN + 1];
	uint8_t ssid_len;
	uint8_t pwd_len;
	uint8_t random;
} sc_result_t;

#define SC_TASK_RUN     (1 << 0)
#define SC_TASK_STOP    (1 << 1)

typedef struct smartconfig_priv {
	struct netif *nif;
	char aes_key[SC_KEY_LEN];
	sc_lead_code_t lead_code;
	SMART_CONFIG_STATUS_T status;
	sc_result_t result;
	uint8_t waiting;
	uint8_t ack_run;
	uint8_t src_data_buff[SC_MAX_SSIDPSK_LEN];
} smartconfig_priv_t;

extern smartconfig_priv_t *smartconfig_priv;

int smart_config_ack_start(smartconfig_priv_t *priv, uint32_t random_num, uint32_t timeout_ms);
int smart_config_ack_stop(smartconfig_priv_t *priv);

#ifdef __cplusplus
}
#endif
#endif /* _SMART_CONFIG__H_ */
