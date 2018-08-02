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

#include "smartlink/airkiss/wlan_airkiss.h"
#include "airkiss.h"

#define g_debuglevel  ERROR

static airkiss_priv_t *airkiss_priv;

static void airkiss_recv_rawframe(uint8_t *data, uint32_t len, void *info)
{
	airkiss_status_t status;
	airkiss_priv_t *priv = airkiss_priv;
	sc_assistant_status sca_status;

	if (!priv) {
		AIRKISS_DBG(ERROR, "%s():%d, priv NULL\n", __func__, __LINE__);
		return;
	}

	if (len < sizeof(struct ieee80211_frame)) {
		AIRKISS_DBG(ERROR, "%s():%d, len %u\n", __func__, __LINE__, len);
		return;
	}

	status = airkiss_recv(&priv->context, data, len);
	sca_status = sc_assistant_get_status();

	if (sca_status == SCA_STATUS_COMPLETE &&
	    priv->status != AIRKISS_STATUS_COMPLETE) {
		AIRKISS_DBG(INFO, "%d should exit status:%d\n", __LINE__, sca_status);
	}
	if (sca_status == SCA_STATUS_CHANNEL_LOCKED &&
	    priv->status != AIRKISS_STATUS_CHANNEL_LOCKED) {
		AIRKISS_DBG(INFO, "%d should exit status:%d\n", __LINE__, sca_status);
	}

	if (status == AIRKISS_STATUS_COMPLETE &&
	    priv->status < AIRKISS_STATUS_COMPLETE) {
		priv->status = status;
		sc_assistant_newstatus(SCA_STATUS_COMPLETE, NULL, info);
		airkiss_get_result(&priv->context, &priv->result);
		AIRKISS_DBG(INFO, "complete ssid:%s pwd:%s random:%d\n",
		            priv->result.ssid, priv->result.pwd, priv->result.random);
	} else if (status == AIRKISS_STATUS_CHANNEL_LOCKED && priv->status != status) {
		priv->status = status;
		AIRKISS_DBG(INFO, "channel locked %d\n", priv->status);
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
		AIRKISS_DBG(ERROR, "%s,%d recv err:%d\n", __func__, __LINE__, status);
}

static void airkiss_reset(airkiss_priv_t *priv)
{
	/* release resource of airkiss */
	airkiss_change_channel(&priv->context);
	priv->result.ssid = NULL;
	priv->result.pwd = NULL;
}

static int airkiss_read_result(const airkiss_result_t *src, wlan_airkiss_result_t *result)
{
	if (!src->ssid) {
		return -1;
	}

	if (src->ssid_len > WLAN_SSID_MAX_LEN) {
		AIRKISS_DBG(ERROR, "Invalid ssid len %d\n", src->ssid_len);
		return -1;
	}

	if (src->pwd_len > WLAN_PASSPHRASE_MAX_LEN) {
		AIRKISS_DBG(ERROR, "Invalid passphrase len %d\n", src->pwd_len);
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

static void wlan_airkiss_sw_ch_cb(struct netif *nif, int16_t channel)
{
	//airkiss_priv_t *priv = (airkiss_priv_t *)arg;
	airkiss_reset(airkiss_priv);
}

static void airkiss_stop(airkiss_priv_t *priv)
{
	AIRKISS_DBG(INFO, "stop\n");

	if (sc_assistant_monitor_unregister_rx_cb(priv->nif, airkiss_recv_rawframe)) {
		AIRKISS_DBG(ERROR, "%s,%d cancel rx cb fail\n", __func__, __LINE__);
	}
	if (sc_assistant_monitor_unregister_sw_ch_cb(priv->nif, wlan_airkiss_sw_ch_cb)) {
		AIRKISS_DBG(ERROR, "%s,%d cancel sw ch cb fail\n", __func__, __LINE__);
	}

	airkiss_reset(priv);
}

static wlan_airkiss_status_t airkiss_start(airkiss_priv_t *priv)
{
	int ret = -1;
	wlan_airkiss_status_t status = WLAN_AIRKISS_SUCCESS;

	if (!priv) {
		return WLAN_AIRKISS_FAIL;
	}

	AIRKISS_DBG(INFO, "start %s\n", airkiss_version());

	//priv->state = AIRKISS_STATUS_SEARCH_CHAN;

	ret = airkiss_init(&priv->context, &priv->func);
	if (ret != 0)
		AIRKISS_DBG(ERROR, "%s airkiss init error\n", __func__);
#if AIRKISS_ENABLE_CRYPT
	if (priv->aes_key[0] != 0) {
		ret = airkiss_set_key(&priv->context,
		                      (const unsigned char *)priv->aes_key,
		                      AK_KEY_LEN);
		if (ret != 0)
			AIRKISS_DBG(ERROR, "%s set key error\n", __func__);
	}
#endif
	ret = sc_assistant_monitor_register_rx_cb(priv->nif, airkiss_recv_rawframe);
	if (ret != 0) {
		AIRKISS_DBG(ERROR, "%s monitor set rx cb fail\n", __func__);
		status = WLAN_AIRKISS_FAIL;
		goto out;
	}
	ret = sc_assistant_monitor_register_sw_ch_cb(priv->nif,
	                                             wlan_airkiss_sw_ch_cb,
	                                             1,
	                                             200);
	if (ret != 0) {
		AIRKISS_DBG(ERROR, "%s monitor sw ch cb fail\n", __func__);
		status = WLAN_AIRKISS_FAIL;
		goto out;
	}

out:
	return status;
}

airkiss_status_t wlan_airkiss_get_status(void)
{
	airkiss_priv_t *priv = airkiss_priv;

	if (!priv)
		return AIRKISS_STATUS_CONTINUE;

	return priv->status;
}

wlan_airkiss_status_t wlan_airkiss_get_result(wlan_airkiss_result_t *result)
{
	wlan_airkiss_status_t ret = WLAN_AIRKISS_SUCCESS;
	airkiss_priv_t *priv = airkiss_priv;

	if (!priv) {
		AIRKISS_DBG(INFO, "airkiss has exit\n");
		return WLAN_AIRKISS_FAIL;
	}

	ret = airkiss_read_result(&priv->result, result);
	if (ret) {
		return ret;
	}

	return ret;
}

wlan_airkiss_status_t wlan_airkiss_start(struct netif *nif, char *key)
{
	airkiss_priv_t *priv = airkiss_priv;

	if (!nif) {
		AIRKISS_DBG(ERROR, "%s, nif NULL!\n", __func__);
		return -1;
	}

	if (priv) {
		AIRKISS_DBG(ERROR, "%s has already started!\n", __func__);
		return -1;
	}

	priv = malloc(sizeof(airkiss_priv_t));
	if (!priv) {
		AIRKISS_DBG(ERROR, "%s malloc failed!\n", __func__);
		return -ENOMEM;
	}
	memset(priv, 0, sizeof(airkiss_priv_t));
	airkiss_priv = priv;

	priv->status = AIRKISS_STATUS_CONTINUE;
	priv->func.memset = (airkiss_memset_fn)&memset;
	priv->func.memcpy = (airkiss_memcpy_fn)&memcpy;
	priv->func.memcmp = (airkiss_memcmp_fn)&memcmp;
	priv->func.printf = (airkiss_printf_fn)&printf;
	priv->nif = nif;

#if AIRKISS_ENABLE_CRYPT
	if (key && strlen(key) == AK_KEY_LEN) {
		memcpy(priv->aes_key, key, AK_KEY_LEN);
	} else if (key) {
		AIRKISS_DBG(ERROR, "%s,%d wrong key\n", __func__, __LINE__);
		goto out;
	}
#endif
	if (airkiss_start(priv)) {
		AIRKISS_DBG(ERROR, "%s,%d err!\n", __func__, __LINE__);
		goto out;
	}

	return 0;

out:
	airkiss_priv = NULL;
	free(priv);
	return -1;
}

wlan_airkiss_status_t wlan_airkiss_wait(uint32_t timeout_ms)
{
	wlan_airkiss_status_t status = WLAN_AIRKISS_SUCCESS;
	uint32_t end_time;
	airkiss_priv_t *priv = airkiss_priv;

	if (!priv)
		return WLAN_AIRKISS_FAIL;

	OS_ThreadSuspendScheduler();
	priv->waiting |= AK_TASK_RUN;
	OS_ThreadResumeScheduler();

	end_time = OS_JiffiesToMSecs(OS_GetJiffies()) + timeout_ms;

	while (!(priv->waiting & AK_TASK_STOP) &&
	       priv->status != AIRKISS_STATUS_COMPLETE &&
	       OS_TimeBefore(OS_JiffiesToMSecs(OS_GetJiffies()), end_time)) {
		OS_MSleep(100);
	}
	if (OS_TimeAfter(OS_JiffiesToMSecs(OS_GetJiffies()), end_time))
		status = WLAN_AIRKISS_TIMEOUT;

	OS_ThreadSuspendScheduler();
	priv->waiting = 0;
	OS_ThreadResumeScheduler();

	return status;
}

wlan_airkiss_status_t
wlan_airkiss_connect_ack(struct netif *nif, uint32_t timeout_ms,
                         wlan_airkiss_result_t *result)
{
	int ret;
	uint8_t *psk;
	airkiss_priv_t *priv = airkiss_priv;

	if (!priv || !result) {
		AIRKISS_DBG(INFO, "airkiss has exit or EINVAL\n");
		return WLAN_AIRKISS_FAIL;
	}

	ret = airkiss_read_result(&priv->result, result);
	if (ret) {
		return ret;
	}

	airkiss_reset(priv);

	priv->nif = sc_assistant_open_sta();

	if (result->passphrase[0] != '\0') {
		psk = result->passphrase;
	} else {
		psk = NULL;
	}

	/* wait timeout_ms not too long for get ssid success */
	sc_assistant_connect_ap(result->ssid, result->ssid_len, psk, timeout_ms);

	ret = airkiss_ack_start(priv, result->random_num, AK_ACK_TIME_OUT_MS);
	if (ret < 0)
		AIRKISS_DBG(ERROR, "airkiss ack error, ap connect time out\n");

	return ret;
}

int wlan_airkiss_stop(void)
{
	airkiss_priv_t *priv = airkiss_priv;

	if (!priv) {
		AIRKISS_DBG(INFO, "%s has already stoped!\n", __func__);
		return -1;
	}

	airkiss_stop(priv);
	airkiss_ack_stop(priv);

	airkiss_priv = NULL; /* all tasks except waitting have exited, set to NULL is ok */

	sc_assistant_stop_connect_ap();

	OS_ThreadSuspendScheduler();
	priv->waiting |= AK_TASK_STOP;
	OS_ThreadResumeScheduler();
	while (priv->waiting & AK_TASK_RUN) {
		OS_MSleep(10);
	}

	free(priv);

	return 0;
}
