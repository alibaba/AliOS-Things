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
#include "cmd_wdg.h"
#include "driver/chip/hal_wdg.h"

static void cmd_wdg_callback(void *arg)
{
	cmd_write_event(CMD_EVENT_WDG_TIMEOUT, "wdg timeout @ %d ms", OS_GetTicks());
}

/*
 * drv wdg config m=<mode> t=<timeout>
 */
static enum cmd_status cmd_wdg_config_exec(char *cmd)
{
	char mode_str[8];
	uint32_t timeout;
	WDG_InitParam param;
	int32_t cnt;

	cnt = cmd_sscanf(cmd, "m=%7s t=%u", mode_str, &timeout);
	if (cnt != 2) {
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_memset(&param, 0, sizeof(param));

	if (cmd_strcmp(mode_str, "reset") == 0) {
		param.event = WDG_EVT_RESET;
		param.resetCycle = WDG_DEFAULT_RESET_CYCLE;
	} else if (cmd_strcmp(mode_str, "it") == 0) {
		param.event = WDG_EVT_INTERRUPT;
		param.callback = cmd_wdg_callback;
		param.arg = NULL;
	} else {
		CMD_ERR("invalid mode %s\n", mode_str);
		return CMD_STATUS_INVALID_ARG;
	}

	if (timeout > 11) {
		CMD_ERR("invalid timeout %u\n", timeout);
		return CMD_STATUS_INVALID_ARG;
	} else {
		param.timeout = (timeout << WDG_TIMEOUT_SHIFT) & WDG_TIMEOUT_MASK;
	}

	if (HAL_WDG_Init(&param) != HAL_OK) {
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

/*
 * drv wdg deconfig
 */
static enum cmd_status cmd_wdg_deconfig_exec(char *cmd)
{
	HAL_WDG_DeInit();
	return CMD_STATUS_OK;
}

/*
 * drv wdg start
 */
static enum cmd_status cmd_wdg_start_exec(char *cmd)
{
	HAL_WDG_Start();
	return CMD_STATUS_OK;
}

/*
 * drv wdg stop
 */
static enum cmd_status cmd_wdg_stop_exec(char *cmd)
{
	HAL_WDG_Stop();
	return CMD_STATUS_OK;
}

/*
 * drv wdg feed
 */
static enum cmd_status cmd_wdg_feed_exec(char *cmd)
{
	HAL_WDG_Feed();
	return CMD_STATUS_OK;
}

/*
 * drv wdg reboot
 */
static enum cmd_status cmd_wdg_reboot_exec(char *cmd)
{
	HAL_WDG_Reboot();
	return CMD_STATUS_OK;
}

static struct cmd_data g_wdg_cmds[] = {
	{ "config",		cmd_wdg_config_exec },
	{ "deconfig", 	cmd_wdg_deconfig_exec },
	{ "start",		cmd_wdg_start_exec },
	{ "stop",		cmd_wdg_stop_exec },
	{ "feed",		cmd_wdg_feed_exec },
	{ "reboot",		cmd_wdg_reboot_exec },
};

enum cmd_status cmd_wdg_exec(char *cmd)
{
	return cmd_exec(cmd, g_wdg_cmds, cmd_nitems(g_wdg_cmds));
}
