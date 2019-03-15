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

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "string.h"
#include "sys/mbuf.h"
#include "kernel/os/os.h"
#include "net/wlan/wlan.h"

#include "net/wlan/wlan_smart_config.h"
#include "smart_config_decode.h"

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define g_debuglevel  ERROR

#define SMART_DBG(level, fmt, args...)                          \
	do {                                                    \
		if (level <= g_debuglevel)                      \
			printf("[smart config]"fmt,##args);     \
	} while (0)

#define MAX_TIME 4294967295
static struct netif *priv_nif = NULL;
SC_Priv *sc_priv;

static uint32_t d_time(uint32_t time1, uint32_t time2)
{
	uint32_t d_time = 0;
	if (time1 <= time2)
		d_time = time2 - time1;
	else
		d_time = MAX_TIME - time1 + time2;
	return d_time;
}

static int smartconfig_wifi_set_channel(struct netif *nif, int16_t ch)
{
	return wlan_monitor_set_channel(nif, ch);
}

static void process_macframe(uint8_t *data, uint32_t len, void *info)
{
	/* make sure frame is qos data frame. */
	SMART_CONFIG_STATUS_T sta = sc_dec_packet_deoced(sc_priv, data);

	if (sta == SC_COMPLETED) {
		SMART_DBG(INFO, "%s(),%d complete\n", __func__, __LINE__);
		sc_priv->sc_ctrl = SC_END;

		if (wlan_monitor_set_rx_cb(priv_nif, NULL) == -1)
			SMART_DBG(ERROR, "%s(),%d monitor set rx cb error\n", __func__, __LINE__);
	} else if (sta == SC_LOCKED_CHAN) {
		sc_priv->sc_ctrl = SC_LOCKED_CHAN;
	}
}

static int smartconfig_read_result(wlan_smart_config_result_t *result)
{
	SC_Result *sc_result = sc_read_result(sc_priv);

	if (sc_result != NULL) {
		if (sc_result->ssid_size > WLAN_SSID_MAX_LEN) {
			SMART_DBG(ERROR, "Invalid ssid len %d\n", sc_result->ssid_size);
			return -1;
		}

		if (sc_result->pwd_size > WLAN_PASSPHRASE_MAX_LEN) {
			SMART_DBG(ERROR, "Invalid passphrase len %d\n", sc_result->pwd_size);
			return -1;
		}

		SMART_DBG(INFO, "smart config: ssid: %.32s, passphrase: %s, random_num: %d\n",
		          sc_result->ssid, sc_result->pwd, sc_result->random_num);

		memcpy(result->ssid, sc_result->ssid, sc_result->ssid_size);
		result->ssid_len = sc_result->ssid_size;

		if (sc_result->pwd_size > 0 && sc_result->pwd != NULL) {
			memcpy(result->passphrase, sc_result->pwd, sc_result->pwd_size);
			result->passphrase[sc_result->pwd_size] = '\0';
		} else {
			result->passphrase[0] = '\0';
		}

		result->random_num = sc_result->random_num;
		return 0;
	}

	return -1;
}

/* the key length must be 16 byte */
static int smartconfig_set_key(const char *key, uint32_t length)
{
	if (!sc_priv) {
		sc_priv = malloc(sizeof(SC_Priv));
		if (!sc_priv) {
			SMART_DBG(ERROR, "%s(),%d, malloc faild!\n", __func__, __LINE__);
			return -1;
		}
		memset(sc_priv, 0, sizeof(SC_Priv));
	}

	return sc_set_aeskey(sc_priv, key, length);
}

static void smartconfig_close(void)
{
	SMART_DBG(INFO, "close\n");

	if (sc_priv) {
		if (wlan_monitor_set_rx_cb(priv_nif, NULL) != 0)
			SMART_DBG(ERROR, "%s(),%d monitor set rx cb fail\n", __func__, __LINE__);

		sc_priv->sc_ctrl = SC_END;
		OS_MSleep(150);
		free(sc_priv);
		sc_priv = NULL;
	}
}

static wlan_smart_config_status_t smartconfig_start(struct netif *nif,
                                                    uint32_t timeout_ms,
                                                    wlan_smart_config_result_t *result)
{
	int ret = -1;
	wlan_smart_config_status_t status = WLAN_SMART_CONFIG_FAIL;
	int16_t channel = 0;
	int16_t channel_burrer[] = {1, 6, 12, 2, 3, 4, 5, 7, 8, 9, 10, 11, 13};

	if (nif == NULL || result == NULL) {
		SMART_DBG(ERROR, "%s(),%d, nif %p, result %p\n", __func__, __LINE__, nif, result);
		return WLAN_SMART_CONFIG_INVALID;
	}

	priv_nif = nif;
	SMART_DBG(INFO, "start\n");

	if (!sc_priv) {
		sc_priv = malloc(sizeof(SC_Priv));
		if (!sc_priv) {
			SMART_DBG(ERROR, "malloc faild!\n");
			return status;
		}
		memset(sc_priv, 0, sizeof(SC_Priv));
	}

	sc_priv->sc_ctrl = SC_SEARCH_CHAN;

	SMART_DBG(INFO, "monitor_register\n");
	ret = wlan_monitor_set_rx_cb(priv_nif, process_macframe);
	if (ret != 0) {
		SMART_DBG(ERROR, "%s(),%d monitor set rx cb fail\n", __func__, __LINE__);
		goto out;
	}

	uint32_t start_time = OS_JiffiesToMSecs(OS_GetJiffies());

	while (1) {
		if (sc_priv->sc_ctrl == SC_SEARCH_CHAN) {
			smartconfig_wifi_set_channel(priv_nif, channel_burrer[channel]);

			if (++channel >= sizeof(channel_burrer))
				channel = 0;
		} else if (sc_priv->sc_ctrl == SC_LOCKED_CHAN) {
			if (sc_read_locked_channel(sc_priv))
				smartconfig_wifi_set_channel(priv_nif, sc_read_locked_channel(sc_priv));

			SMART_DBG(INFO, "channel locked\n");
			while (sc_priv->sc_ctrl) {
				OS_MSleep(10);
			}
			break;
		} else if (sc_priv->sc_ctrl == SC_END) {
			break;
		}
		uint32_t d_t = d_time(start_time, OS_JiffiesToMSecs(OS_GetJiffies()));
		if (d_t >= timeout_ms) {
			smartconfig_close();
			status = WLAN_SMART_CONFIG_TIMEOUT;
			goto out;
		}

		OS_MSleep(100);
	}

	ret = smartconfig_read_result(result);
	if (ret == 0) {
		status = WLAN_SMART_CONFIG_SUCCESS;
	}

out:
	if (sc_priv) {
		free(sc_priv);
		sc_priv = NULL;
	}
	SMART_DBG(INFO, "end\n");
	return status;
}

int wlan_smart_config_set_key(const char *key, uint32_t len)
{
	if (key == NULL || len != WLAN_SMART_CONFIG_KEY_LEN) {
		SMART_DBG(ERROR, "invalid smart config key (%p, %d)\n", key, len);
		return -1;
	}

	return smartconfig_set_key(key, len);
}

wlan_smart_config_status_t wlan_smart_config_start(struct netif *nif,
                                                   uint32_t timeout_ms,
                                                   wlan_smart_config_result_t *result)
{
	return smartconfig_start(nif, timeout_ms, result);
}

int wlan_smart_config_stop(void)
{
	smartconfig_close();
	return 0;
}
