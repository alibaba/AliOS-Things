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
#include "smartlink/sc_assistant.h"

#include "smartlink/smart_config/wlan_smart_config.h"
#include "smart_config.h"
#include "smart_config_decode.h"

#define g_debuglevel  ERROR

smartconfig_priv_t *smartconfig_priv;
const char smartconfig_version_str[] = { "1.0" };

const char *smartconfig_version(void)
{
	return smartconfig_version_str;
}

static void smartconfig_recv_rawframe(uint8_t *data, uint32_t len, void *info)
{
	SMART_CONFIG_STATUS_T status;
	smartconfig_priv_t *priv = smartconfig_priv;
	sc_assistant_status sca_status;

	if (!priv) {
		SMART_DBG(ERROR, "%s():%d, priv NULL\n", __func__, __LINE__);
		return;
	}

	if (len < sizeof(struct ieee80211_frame)) {
		SMART_DBG(ERROR, "%s():%d, len %u\n", __func__, __LINE__, len);
		return;
	}

	status = sc_dec_packet_decode(priv, data, len);
	sca_status = sc_assistant_get_status();

	if (sca_status == SCA_STATUS_COMPLETE &&
	    priv->status != SC_STATUS_COMPLETE) {
		SMART_DBG(INFO, "%d should exit status:%d\n", __LINE__, sca_status);
	}
	if (sca_status == SCA_STATUS_CHANNEL_LOCKED &&
	    priv->status != SC_STATUS_LOCKED_CHAN) {
		SMART_DBG(INFO, "%d should exit status:%d\n", __LINE__, sca_status);
	}

	if (status == SC_STATUS_COMPLETE &&
	    priv->status < SC_STATUS_COMPLETE) {
		priv->status = status;
		sc_assistant_newstatus(SCA_STATUS_COMPLETE, NULL, info);
		SMART_DBG(INFO, "complete ssid: %s pwd: %s random: %d\n",
		          priv->result.ssid, priv->result.pwd, priv->result.random);
	} else if (status == SC_STATUS_LOCKED_CHAN && priv->status != status) {
		priv->status = status;
		SMART_DBG(INFO, "channel locked\n");
		uint8_t *ap_mac;
		int dir = data[1] & IEEE80211_FC1_DIR_MASK;

		if (dir == IEEE80211_FC1_DIR_NODS) {
			ap_mac = &data[16];
		} else if (dir == IEEE80211_FC1_DIR_FROMDS) {
			ap_mac = &data[10];
		} else {
			ap_mac = &data[4];
		}
		sc_assistant_newstatus(SCA_STATUS_CHANNEL_LOCKED, ap_mac, info);
	} else if (status < 0)
		SMART_DBG(ERROR, "%s,%d recv err:%d\n", __func__, __LINE__, status);
}

static int smartconfig_read_result(sc_result_t *src, wlan_smart_config_result_t *result)
{
	if (!src->ssid) {
		return -1;
	}

	if (src->ssid_len > WLAN_SSID_MAX_LEN) {
		SMART_DBG(ERROR, "Invalid ssid len %d\n", src->ssid_len);
		return -1;
	}

	if (src->pwd_len > WLAN_PASSPHRASE_MAX_LEN) {
		SMART_DBG(ERROR, "Invalid passphrase len %d\n", src->pwd_len);
		return -1;
	}

	if (src->ssid_len <= WLAN_SSID_MAX_LEN) {
		memcpy(result->ssid, src->ssid, src->ssid_len);
		result->ssid_len = src->ssid_len;
	} else {
		result->ssid[0] = '\0';
		result->ssid_len = 0;
	}

	if (src->pwd_len > 0 && src->pwd != NULL) {
		memcpy(result->passphrase, src->pwd, src->pwd_len);
		result->passphrase[src->pwd_len] = '\0';
	} else {
		result->passphrase[0] = '\0';
	}

	result->random_num = src->random;
	return 0;
}

static void smartconfig_sw_ch_cb(struct netif *nif, int16_t channel)
{
	/* do nothing */
}

static void smartconfig_stop(smartconfig_priv_t *priv)
{
	SMART_DBG(INFO, "stop\n");

	if (sc_assistant_monitor_unregister_rx_cb(priv->nif, smartconfig_recv_rawframe)) {
		SMART_DBG(ERROR, "%s,%d cancel rx cb fail\n", __func__, __LINE__);
	}
	if (sc_assistant_monitor_unregister_sw_ch_cb(priv->nif, smartconfig_sw_ch_cb)) {
		SMART_DBG(ERROR, "%s,%d cancel sw ch cb fail\n", __func__, __LINE__);
	}

	priv->status = SC_STATUS_END;
}

static wlan_smart_config_status_t smartconfig_start(smartconfig_priv_t *priv)
{
	int ret = -1;
	wlan_smart_config_status_t status = WLAN_SMART_CONFIG_SUCCESS;

	if (!priv) {
		return WLAN_SMART_CONFIG_FAIL;
	}

	SMART_DBG(INFO, "start %s\n", smartconfig_version());

	priv->status = SC_STATUS_SEARCH_CHAN;

	ret = sc_assistant_monitor_register_rx_cb(priv->nif, smartconfig_recv_rawframe);
	if (ret) {
		SMART_DBG(ERROR, "%s monitor set rx cb fail\n", __func__);
		status = WLAN_SMART_CONFIG_FAIL;
		goto out;
	}
	ret = sc_assistant_monitor_register_sw_ch_cb(priv->nif,
	                                             smartconfig_sw_ch_cb,
	                                             1,
	                                             200);
	if (ret) {
		SMART_DBG(ERROR, "%s monitor sw ch cb fail\n", __func__);
		status = WLAN_SMART_CONFIG_FAIL;
		goto out;
	}

out:
	return status;
}

SMART_CONFIG_STATUS_T wlan_smart_config_get_status(void)
{
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!priv)
		return SC_STATUS_END;

	return priv->status;
}

wlan_smart_config_status_t smartconfig_get_result(wlan_smart_config_result_t *result)
{
	wlan_smart_config_status_t ret = WLAN_SMART_CONFIG_SUCCESS;
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!priv) {
		SMART_DBG(INFO, "airkiss has exit\n");
		return WLAN_SMART_CONFIG_FAIL;
	}

	ret = smartconfig_read_result(&priv->result, result);
	if (ret) {
		return WLAN_SMART_CONFIG_FAIL;
	}

	return ret;
}

wlan_smart_config_status_t wlan_smart_config_start(struct netif *nif, char *key)
{
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!nif) {
		SMART_DBG(ERROR, "%s, nif NULL!\n", __func__);
		return -1;
	}

	if (priv) {
		SMART_DBG(ERROR, "%s has already started!\n", __func__);
		return -1;
	}

	priv = malloc(sizeof(smartconfig_priv_t));
	if (!priv) {
		SMART_DBG(ERROR, "%s malloc failed!\n", __func__);
		return -ENOMEM;
	}
	memset(priv, 0, sizeof(smartconfig_priv_t));
	smartconfig_priv = priv;
	sc_reset_lead_code(&priv->lead_code);

	priv->status = SC_STATUS_SEARCH_CHAN;
	priv->nif = nif;

#if SMARTCONFIG_ENABLE_CRYPT
	if (key && strlen(key) == SC_KEY_LEN){
		memcpy(priv->aes_key, key, SC_KEY_LEN);
	} else if (key) {
		SMART_DBG(ERROR, "%s,%d wrong key\n", __func__, __LINE__);
		goto out;
	}
#endif
	if (smartconfig_start(priv)) {
		SMART_DBG(ERROR, "%s,%d err!\n", __func__, __LINE__);
		goto out;
	}

	return 0;

out:
	smartconfig_priv = NULL;
	free(priv);
	return -1;
}

wlan_smart_config_status_t wlan_smart_config_wait(uint32_t timeout_ms)
{
	SMART_CONFIG_STATUS_T status = WLAN_SMART_CONFIG_SUCCESS;
	uint32_t end_time;
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!priv)
		return WLAN_SMART_CONFIG_FAIL;

	OS_ThreadSuspendScheduler();
	priv->waiting |= SC_TASK_RUN;
	OS_ThreadResumeScheduler();

	end_time = OS_JiffiesToMSecs(OS_GetJiffies()) + timeout_ms;

	while (!(priv->waiting & SC_TASK_STOP) &&
	       priv->status != SC_STATUS_COMPLETE &&
	       OS_TimeBefore(OS_JiffiesToMSecs(OS_GetJiffies()), end_time)) {
		OS_MSleep(100);
	}
	if (OS_TimeAfter(OS_JiffiesToMSecs(OS_GetJiffies()), end_time))
		status = WLAN_SMART_CONFIG_TIMEOUT;

	OS_ThreadSuspendScheduler();
	priv->waiting = 0;
	OS_ThreadResumeScheduler();

	return status;
}

wlan_smart_config_status_t
wlan_smart_config_connect_ack(struct netif *nif, uint32_t timeout_ms,
                        wlan_smart_config_result_t *result)
{
	int ret;
	uint8_t *psk;
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!priv) {
		SMART_DBG(INFO, "airkiss has exit\n");
		return WLAN_SMART_CONFIG_FAIL;
	}

	ret = smartconfig_read_result(&priv->result, result);
	if (ret) {
		return WLAN_SMART_CONFIG_FAIL;
	}

	priv->nif = sc_assistant_open_sta();

	if (result->passphrase[0] != '\0') {
		psk = result->passphrase;
	} else {
		psk = NULL;
	}

	/* wait timeout_ms not too long for get ssid success */
	sc_assistant_connect_ap(result->ssid, result->ssid_len, psk, timeout_ms);

	ret = smart_config_ack_start(priv, result->random_num, SC_ACK_TIME_OUT);
	if (ret < 0)
		SMART_DBG(ERROR, "smartconfig ack error, timeout\n");

	return ret;
}

int wlan_smart_config_stop(void)
{
	smartconfig_priv_t *priv = smartconfig_priv;

	if (!priv) {
		SMART_DBG(ERROR, "%s has already stoped!\n", __func__);
		return -1;
	}

	smartconfig_stop(priv);
	smart_config_ack_stop(priv);

	smartconfig_priv = NULL; /* all tasks except waitting have exited, set to NULL is ok */

	sc_assistant_stop_connect_ap();

	OS_ThreadSuspendScheduler();
	priv->waiting |= SC_TASK_STOP;
	OS_ThreadResumeScheduler();
	while (priv->waiting & SC_TASK_RUN) {
		OS_MSleep(10);
	}

	free(priv);

	return 0;
}
