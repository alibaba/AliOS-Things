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

#define SC_TIME_OUT 120000
#define SC_ACK_TIME_OUT 30000
static char *key = "1234567812345678";

static OS_Thread_t g_sc_ctrl_thread;
#define SC_CTRL_THREAD_STACK_SIZE	(1 * 1024)

static void sc_task(void *arg)
{
	int ret;
	wlan_smart_config_status_t status;
	wlan_smart_config_result_t result;

	memset(&result, 0, sizeof(result));

	net_switch_mode(WLAN_MODE_MONITOR);

	status = wlan_smart_config_start(g_wlan_netif, SC_TIME_OUT, &result);

	net_switch_mode(WLAN_MODE_STA);

	if (status == WLAN_SMART_CONFIG_SUCCESS) {
		CMD_DBG("ssid: %.32s\n", (char *)result.ssid);
		CMD_DBG("psk: %s\n", (char *)result.passphrase);
		CMD_DBG("random: %d\n", result.random_num);
	} else {
		CMD_DBG ("smartconfig failed %d\n", status);
		OS_ThreadDelete(&g_sc_ctrl_thread);
		return;
	}

	if (result.passphrase[0] != '\0') {
		wlan_sta_set(result.ssid, result.ssid_len, result.passphrase);
	} else {
		wlan_sta_set(result.ssid, result.ssid_len, NULL);
	}

	wlan_sta_enable();

	ret = wlan_smart_config_ack_start(g_wlan_netif, result.random_num, SC_ACK_TIME_OUT);
	if (ret < 0)
		CMD_ERR("smartconfig ack error, timeout\n");

	OS_ThreadDelete(&g_sc_ctrl_thread);
}

static int sc_create()
{
	if (OS_ThreadCreate(&g_sc_ctrl_thread,
	        	            "sc_thread",
	            	        sc_task,
	                	    NULL,
	                    	OS_THREAD_PRIO_APP,
	                    	SC_CTRL_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create sc thread failed\n");
		return -1;
	}
	return 0;
}

enum cmd_status cmd_smart_config_exec(char *cmd)
{
	int ret;

	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}

	if (cmd_strcmp(cmd, "start") == 0) {
		if (OS_ThreadIsValid(&g_sc_ctrl_thread)) {
			CMD_ERR("smartconfig already start\n");
			ret = -1;
		} else {
			ret = sc_create();
		}
	} else if (cmd_strcmp(cmd, "stop") == 0) {
		ret = wlan_smart_config_stop();
	} else if (cmd_strcmp(cmd, "set_key") == 0) {
		ret = wlan_smart_config_set_key(key, WLAN_SMART_CONFIG_KEY_LEN);
		CMD_DBG("Smartconfig set key : %s\n", key);
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	return (ret == 0 ? CMD_STATUS_OK : CMD_STATUS_FAIL);
}
