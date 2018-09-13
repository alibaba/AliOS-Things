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

#include "cmd_util.h"
#include "common/framework/net_ctrl.h"
#include "net/wlan/wlan.h"
#include "net/wlan/wlan_defs.h"
#include <string.h>

#define AK_TIME_OUT_MS 120000
#define AK_ACK_TIME_OUT_MS 30000
static char *key = "1234567812345678";

static OS_Thread_t g_ak_ctrl_thread;
#define AK_CTRL_THREAD_STACK_SIZE	(1 * 1024)

static void ak_task(void *arg)
{
	int ret;
	wlan_airkiss_status_t status;
	wlan_airkiss_result_t result;

	memset(&result, 0, sizeof(result));

	net_switch_mode(WLAN_MODE_MONITOR);

	status = wlan_airkiss_start(g_wlan_netif, AK_TIME_OUT_MS, &result);

	net_switch_mode(WLAN_MODE_STA);

	if (status == WLAN_AIRKISS_SUCCESS) {
		CMD_DBG("ssid: %.32s\n", (char *)result.ssid);
		CMD_DBG("psk: %s\n", (char *)result.passphrase);
		CMD_DBG("random: %d\n", result.random_num);
	} else {
		CMD_DBG ("airkiss failed %d\n", status);
		OS_ThreadDelete(&g_ak_ctrl_thread);
		return;
	}

	if (result.passphrase[0] != '\0') {
		wlan_sta_set(result.ssid, result.ssid_len, result.passphrase);
	} else {
		wlan_sta_set(result.ssid, result.ssid_len, NULL);
	}

	wlan_sta_enable();

	ret = wlan_airkiss_ack_start(g_wlan_netif, result.random_num, AK_ACK_TIME_OUT_MS);
	if (ret < 0)
		CMD_DBG("airkiss ack error, ap connect time out\n");

	OS_ThreadDelete(&g_ak_ctrl_thread);
}

static int ak_create()
{
	if (OS_ThreadCreate(&g_ak_ctrl_thread,
	        	            "ak_thread",
	            	        ak_task,
	                	    NULL,
	                    	OS_THREAD_PRIO_APP,
	                    	AK_CTRL_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create ak thread failed\n");
		return -1;
	}
	return 0;
}

enum cmd_status cmd_airkiss_exec(char *cmd)
{
	int ret = 0;

	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}

	if (cmd_strcmp(cmd, "start") == 0) {
		if (OS_ThreadIsValid(&g_ak_ctrl_thread)) {
			CMD_ERR("Airkiss is already start\n");
			ret = -1;
		} else {
			ret = ak_create();
		}
	} else if (cmd_strcmp(cmd, "set_key") == 0) {
		ret = wlan_airkiss_set_key(key, WLAN_AIRKISS_KEY_LEN);
		CMD_DBG("Airkiss set key : %s\n", key);
	} else if (cmd_strcmp(cmd, "stop") == 0) {
		ret = wlan_airkiss_stop();
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	return (ret == 0 ? CMD_STATUS_OK : CMD_STATUS_FAIL);
}
