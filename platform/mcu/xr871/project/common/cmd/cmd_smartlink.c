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

#include <string.h>

#include "net/wlan/wlan.h"
#include "net/wlan/wlan_defs.h"
#include "lwip/netif.h"

#include "cmd_util.h"
#include "common/framework/net_ctrl.h"

#include "smartlink/sc_assistant.h"
#include "common/cmd/cmd_smartlink.h"
#include "smartlink/smart_config/wlan_smart_config.h"
#include "smartlink/airkiss/wlan_airkiss.h"
#include "smartlink/voice_print/voice_print.h"

#define SMARTLINK_USE_AIRKISS
#define SMARTLINK_USE_SMARTCONFIG
#define SMARTLINK_USE_VOICEPRINT

#define SMARTLINK_TIME_OUT_MS 120000

#ifdef SMARTLINK_USE_AIRKISS
static int ak_key_used;
static char *airkiss_key = "1234567812345678";
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
static int sc_key_used;
static char *smartconfig_key = "1234567812345678";
#endif

#define SL_TASK_RUN     (1 << 0)
#define SL_TASK_STOP    (1 << 1)

static int thread_run;

static OS_Thread_t g_thread;
#define THREAD_STACK_SIZE       (2 * 1024)

#ifdef SMARTLINK_USE_VOICEPRINT
uint8_t sc_vp_checksum(uint8_t * buf, int len)
{
	int i;
	uint8_t cs = 0;

	for (i = 0;i < len;i++) {
		cs += buf[i];
	}
	return cs;
}

static int sc_vp_result_handler(char * result_str, wlan_voiceprint_result_t * vp_result)
{
	const char *str_find;
	char temp[3] = {0};
	uint8_t len_temp, cs;

	str_find = result_str;
	memcpy(temp, str_find, 2);
	vp_result->ssid_len = strtol(temp, NULL, 16);
	str_find += 2;
	strncpy((char *)vp_result->ssid, str_find, vp_result->ssid_len);
	vp_result->ssid[vp_result->ssid_len] = 0;
	CMD_DBG("SSID len:%d, %s\n", vp_result->ssid_len, vp_result->ssid);

	str_find += vp_result->ssid_len;
	memcpy(temp, str_find, 2);
	len_temp = strtol(temp, NULL, 16);
	str_find += 2;
	strncpy((char *)vp_result->passphrase, str_find, len_temp);
	vp_result->passphrase[len_temp] = 0;
	CMD_DBG("PSK len:%d, %s\n", len_temp, vp_result->passphrase);

	str_find += len_temp;
	memcpy(temp, str_find, 2);
	cs = strtol(temp, NULL, 16);
	len_temp = 2 + vp_result->ssid_len + 2 + len_temp;
	cs += sc_vp_checksum((uint8_t *)result_str, len_temp);

	if (0xFF != cs) {
		CMD_DBG("cs ERROR:%d\n", cs);
		vp_result->ssid_len = 0;
		vp_result->ssid[0] = 0;
		vp_result->passphrase[0] = 0;
		return -1;
	}

	CMD_DBG("SSID:%.*s PSK:%s\n", vp_result->ssid_len, vp_result->ssid,
		   vp_result->passphrase);
	return 0;
}
#endif

static void smartlink_task(void *arg)
{
#ifdef SMARTLINK_USE_AIRKISS
	wlan_airkiss_result_t ak_result;
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	wlan_smart_config_result_t sc_result;
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	wlan_voiceprint_result_t vp_result;
#endif
	uint32_t end_time;

#ifdef SMARTLINK_USE_AIRKISS
	memset(&ak_result, 0, sizeof(wlan_airkiss_result_t));
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	memset(&sc_result, 0, sizeof(wlan_smart_config_result_t));
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	memset(&vp_result, 0, sizeof(wlan_voiceprint_result_t));
#endif

	CMD_DBG("%s getting ssid and psk...\n", __func__);

	OS_ThreadSuspendScheduler();
	thread_run |= SL_TASK_RUN;
	OS_ThreadResumeScheduler();

	end_time = OS_JiffiesToMSecs(OS_GetJiffies()) + SMARTLINK_TIME_OUT_MS;
	while (!(thread_run & SL_TASK_STOP) &&
	       OS_TimeBefore(OS_JiffiesToMSecs(OS_GetJiffies()), end_time) &&
	       sc_assistant_get_status() < SCA_STATUS_COMPLETE) {
#ifdef SMARTLINK_USE_VOICEPRINT
		voice_print_wait_once();
#else
		OS_MSleep(100);
#endif
	}
	if (OS_TimeAfter(OS_JiffiesToMSecs(OS_GetJiffies()), end_time)) {
		goto out;
	}
	CMD_DBG("%s get ssid and psk finished\n", __func__);

#ifdef SMARTLINK_USE_AIRKISS
	if (wlan_airkiss_get_status() == AIRKISS_STATUS_COMPLETE) {
		if (!wlan_airkiss_connect_ack(g_wlan_netif, SMARTLINK_TIME_OUT_MS, &ak_result)) {
			CMD_DBG("ssid:%s psk:%s random:%d\n", (char *)ak_result.ssid,
			        (char *)ak_result.passphrase, ak_result.random_num);
		}
	}
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	if (wlan_smart_config_get_status() == SC_STATUS_COMPLETE) {
		if (!wlan_smart_config_connect_ack(g_wlan_netif, SMARTLINK_TIME_OUT_MS, &sc_result)) {
			CMD_DBG("ssid:%s psk:%s random:%d\n", (char *)sc_result.ssid,
			        (char *)sc_result.passphrase, sc_result.random_num);
		}
	}
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	{
		char result[200];
		int ret, len;
		uint8_t *psk;
		len = 200;
		cmd_memset(result, 0, len);
		if (voiceprint_get_status() == VP_STATUS_COMPLETE) {
			if (wlan_voiceprint_get_raw_result(result, &len) == WLAN_VOICEPRINT_SUCCESS) {
				CMD_DBG("string:%s len:%d\n", result, len);
				if (!sc_vp_result_handler(result, &vp_result)) {
					g_wlan_netif = sc_assistant_open_sta();
					if (vp_result.passphrase[0] != '\0') {
						psk = vp_result.passphrase;
					} else {
						psk = NULL;
					}
					ret = sc_assistant_connect_ap(vp_result.ssid, vp_result.ssid_len,
													psk, SMARTLINK_TIME_OUT_MS);
					if (ret < 0) {
						CMD_DBG("voiceprint connect ap time out\n");
						goto out;
					}
#if 0
					ret = voiceprint_ack_start(priv, result->random_num, VP_ACK_TIME_OUT_MS);
					if (ret < 0)
						VP_DBG(ERROR, "voice ack error, ap connect time out\n");
#endif
				}
				CMD_DBG("ssid:%s psk:%s random:%d\n", (char *)vp_result.ssid,
				        (char *)vp_result.passphrase, vp_result.random_num);
			}
		}
	}
#endif

out:
#ifdef SMARTLINK_USE_AIRKISS
	wlan_airkiss_stop();
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	wlan_smart_config_stop();
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	voice_print_stop(0);
#endif
	sc_assistant_deinit(g_wlan_netif);

	OS_ThreadSuspendScheduler();
	thread_run = 0;
	OS_ThreadResumeScheduler();

	OS_ThreadDelete(&g_thread);
}

static int smartlink_start(void)
{
	int ret = 0;
#ifdef SMARTLINK_USE_AIRKISS
	wlan_airkiss_status_t ak_status;
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	wlan_smart_config_status_t sc_status;
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	voiceprint_ret_t vp_status;
#endif
	sc_assistant_fun_t sca_fun;
	sc_assistant_time_config_t config;

	if (OS_ThreadIsValid(&g_thread))
		return -1;

	sc_assistant_get_fun(&sca_fun);
	config.time_total = SMARTLINK_TIME_OUT_MS;
	config.time_sw_ch_long = 400;
	config.time_sw_ch_short = 100;
	sc_assistant_init(g_wlan_netif, &sca_fun, &config);

#ifdef SMARTLINK_USE_AIRKISS
	ak_status = wlan_airkiss_start(g_wlan_netif, ak_key_used ? airkiss_key : NULL);
	if (ak_status != WLAN_AIRKISS_SUCCESS) {
		CMD_DBG("airkiss start fiald!\n");
	}
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	sc_status = wlan_smart_config_start(g_wlan_netif, sc_key_used ? smartconfig_key : NULL);
	if (sc_status != WLAN_SMART_CONFIG_SUCCESS) {
		CMD_DBG("smartconfig start fiald!\n");
	}
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	vp_status = voice_print_start(g_wlan_netif, NULL);
	if (vp_status != WLAN_VOICEPRINT_SUCCESS) {
		CMD_DBG("voiceprint start fiald!\n");
	}
#endif
	OS_ThreadSuspendScheduler();
	thread_run = 0;
	OS_ThreadResumeScheduler();

	if (OS_ThreadCreate(&g_thread,
	                    "cmd_sl",
	                    smartlink_task,
	                    NULL,
	                    OS_THREAD_PRIO_APP,
	                    THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create smartlink thread failed\n");
		ret = -1;
	}
	return ret;
}

static int smartlink_stop(void)
{
	if (!OS_ThreadIsValid(&g_thread))
		return -1;

#ifdef SMARTLINK_USE_AIRKISS
	wlan_airkiss_stop();
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	wlan_smart_config_stop();
#endif
#ifdef SMARTLINK_USE_VOICEPRINT
	voice_print_stop(1);
#endif

	OS_ThreadSuspendScheduler();
	thread_run |= SL_TASK_STOP;
	OS_ThreadResumeScheduler();

	while (OS_ThreadIsValid(&g_thread)) {
		OS_MSleep(5);
	}

	return 0;
}

enum cmd_status cmd_smartlink_exec(char *cmd)
{
	int ret = 0;
	char *str_key;

	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}

#ifdef SMARTLINK_USE_AIRKISS
	str_key = cmd_strstr(cmd, "set_airkiss_key");
	if (str_key != NULL) {
		str_key += cmd_strlen("set_airkiss_key");
		if (*str_key != '\0') {
			str_key ++;//skip the space
			if (cmd_strlen(str_key) == 0) {
				ak_key_used = 1;
			} else if (cmd_strlen(str_key) == cmd_strlen(airkiss_key)) {
				cmd_memcpy(airkiss_key, str_key, cmd_strlen(airkiss_key));
				ak_key_used = 1;
			} else {
				CMD_ERR("invalid argument '%s',len:%d, str:%s\n", cmd, cmd_strlen(str_key), str_key);
				return CMD_STATUS_INVALID_ARG;
			}
		} else {
			ak_key_used = 1;
		}
		CMD_DBG("Airkiss set key : %s\n", airkiss_key);
		goto out;
	}
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
	str_key = cmd_strstr(cmd, "set_smartconfig_key");
	if (str_key != NULL) {
		str_key += cmd_strlen("set_smartconfig_key");
		if (*str_key != '\0') {
			str_key ++;//skip the space
			if (cmd_strlen(str_key) == 0) {
				sc_key_used = 1;
			} else if (cmd_strlen(str_key) == cmd_strlen(smartconfig_key)) {
				cmd_memcpy(smartconfig_key, str_key, cmd_strlen(smartconfig_key));
				sc_key_used = 1;
			} else {
				CMD_ERR("invalid argument '%s'\n", cmd);
				return CMD_STATUS_INVALID_ARG;
			}
		} else {
			sc_key_used = 1;
		}
		CMD_DBG("Smartconfig set key : %s\n", smartconfig_key);
		goto out;
	}
#endif
	if (cmd_strcmp(cmd, "start") == 0) {
		if (OS_ThreadIsValid(&g_thread)) {
			CMD_ERR("Smartlink is already start\n");
			ret = -1;
		} else {
			ret = smartlink_start();
		}
	} else if (cmd_strcmp(cmd, "stop") == 0) {
		ret = smartlink_stop();
#ifdef SMARTLINK_USE_AIRKISS
		ak_key_used = 0;
#endif
#ifdef SMARTLINK_USE_SMARTCONFIG
		sc_key_used = 0;
#endif
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

out:
	return (ret == 0 ? CMD_STATUS_OK : CMD_STATUS_FAIL);
}
