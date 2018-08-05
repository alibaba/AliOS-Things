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
#include "smartlink/smart_config/wlan_smart_config.h"

#define SC_TIME_OUT 120000

static int sc_key_used;
static char *sc_key = "1234567812345678";

static OS_Thread_t g_thread;
#define THREAD_STACK_SIZE       (1 * 1024)

static void sc_task(void *arg)
{
	wlan_smart_config_result_t sc_result;

	memset(&sc_result, 0, sizeof(wlan_smart_config_result_t));

	CMD_DBG("%s getting ssid and psk...\n", __func__);

	if (wlan_smart_config_wait(SC_TIME_OUT) == WLAN_SMART_CONFIG_TIMEOUT) {
		goto out;
	}
	CMD_DBG("%s get ssid and psk finished\n", __func__);

	if (wlan_smart_config_get_status() == SC_STATUS_COMPLETE) {
		wlan_smart_config_connect_ack(g_wlan_netif, SC_TIME_OUT, &sc_result);
		CMD_DBG("ssid:%s psk:%s random:%d\n", (char *)sc_result.ssid,
		        (char *)sc_result.passphrase, sc_result.random_num);
	}

out:
	wlan_smart_config_stop();
	sc_assistant_deinit(g_wlan_netif);
	OS_ThreadDelete(&g_thread);
}

static int cmd_sc_start(void)
{
	wlan_smart_config_status_t sc_status;
	sc_assistant_fun_t sca_fun;
	sc_assistant_time_config_t config;

	if (OS_ThreadIsValid(&g_thread))
		return -1;

	sc_assistant_get_fun(&sca_fun);
	config.time_total = SC_TIME_OUT;
	config.time_sw_ch_long = 400;
	config.time_sw_ch_short = 100;
	sc_assistant_init(g_wlan_netif, &sca_fun, &config);

	sc_status = wlan_smart_config_start(g_wlan_netif, sc_key_used ? sc_key : NULL);
	if (sc_status != WLAN_SMART_CONFIG_SUCCESS) {
		CMD_DBG("smartconfig start fiald!\n");
		goto out;
	}

	if (OS_ThreadCreate(&g_thread,
	                    "cmd_sc",
	                    sc_task,
	                    NULL,
	                    OS_THREAD_PRIO_APP,
	                    THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create sc thread failed\n");
		goto out;
	}
	return 0;
out:
	return -1;
}

static int cmd_sc_stop(void)
{
	if (!OS_ThreadIsValid(&g_thread))
		return -1;

	return wlan_smart_config_stop();
}

enum cmd_status cmd_smart_config_exec(char *cmd)
{
	int ret = 0;
	char *str_key;

	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}

	str_key = cmd_strstr(cmd, "set_key");
	if (str_key != NULL) {
		str_key += cmd_strlen("set_key");
		if (*str_key != '\0') {
			str_key ++;//skip the space
			if (cmd_strlen(str_key) == 0) {
				sc_key_used = 1;
			} else if (cmd_strlen(str_key) == cmd_strlen(sc_key)) {
				cmd_memcpy(sc_key, str_key, cmd_strlen(sc_key));
				sc_key_used = 1;
			} else {
				CMD_ERR("invalid argument '%s'\n", cmd);
				return CMD_STATUS_INVALID_ARG;
			}
		} else {
			sc_key_used = 1;
		}
		CMD_DBG("Smartconfig set key : %s\n", sc_key);
		goto out;
	}
	if (cmd_strcmp(cmd, "start") == 0) {
		if (OS_ThreadIsValid(&g_thread)) {
			CMD_ERR("Smartconfig already start\n");
			ret = -1;
		} else {
			ret = cmd_sc_start();
		}
	} else if (cmd_strcmp(cmd, "stop") == 0) {
		ret = cmd_sc_stop();
		sc_key_used = 0;
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

out:
	return (ret == 0 ? CMD_STATUS_OK : CMD_STATUS_FAIL);
}
