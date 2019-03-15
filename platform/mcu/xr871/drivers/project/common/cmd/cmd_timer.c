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
#include "cmd_timer.h"
#include "driver/chip/hal_timer.h"

enum cmd_timer_action {
	CMD_TIMER_ACTION_DECONFIG,
	CMD_TIMER_ACTION_START,
	CMD_TIMER_ACTION_STOP,
	CMD_TIMER_ACTION_PAUSE,
	CMD_TIMER_ACTION_CONTINUE,
	CMD_TIMER_ACTION_VALUE,
};

static void cmd_timer_callback(void *arg)
{
	cmd_write_event(CMD_EVENT_TIMER_NOTIFY, "timer %d notify @ %d ms",
	                (int)arg, OS_GetTicks());
}

/*
 * drv timer config i=<id> m=<mode> s=<clk-src> d=<clk-div> p=<period>
 */
static enum cmd_status cmd_timer_config_exec(char *cmd)
{
	uint32_t id, clk_div, period;
	char mode_str[8], clk_src_str[8];
	TIMER_Mode mode;
	TIMER_ClkSrc clk_src;
	TIMER_ClkPrescaler clk_prescaler;
	TIMER_InitParam param;
	int32_t cnt;

	cnt = cmd_sscanf(cmd, "i=%u m=%7s s=%7s d=%u p=%u",
	                 &id, mode_str, clk_src_str, &clk_div, &period);
	if (cnt != 5) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= TIMER_NUM) {
		CMD_ERR("invalid id %d\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(mode_str, "repeat") == 0) {
		mode = TIMER_MODE_REPEAT;
	} else if (cmd_strcmp(mode_str, "once") == 0) {
		mode = TIMER_MODE_ONCE;
	} else {
		CMD_ERR("invalid mode %s\n", mode_str);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(clk_src_str, "LF") == 0) {
		clk_src = TIMER_CLK_SRC_LFCLK;
	} else if (cmd_strcmp(clk_src_str, "HF") == 0) {
		clk_src = TIMER_CLK_SRC_HFCLK;
	} else {
		CMD_ERR("invalid clk-src %s\n", clk_src_str);
		return CMD_STATUS_INVALID_ARG;
	}

	switch (clk_div) {
	case 1:
		clk_prescaler = TIMER_CLK_PRESCALER_1;
		break;
	case 2:
		clk_prescaler = TIMER_CLK_PRESCALER_2;
		break;
	case 4:
		clk_prescaler = TIMER_CLK_PRESCALER_4;
		break;
	case 8:
		clk_prescaler = TIMER_CLK_PRESCALER_8;
		break;
	case 16:
		clk_prescaler = TIMER_CLK_PRESCALER_16;
		break;
	case 32:
		clk_prescaler = TIMER_CLK_PRESCALER_32;
		break;
	case 64:
		clk_prescaler = TIMER_CLK_PRESCALER_64;
		break;
	case 128:
		clk_prescaler = TIMER_CLK_PRESCALER_128;
		break;
	default:
		CMD_ERR("invalid clk-div %u\n", clk_div);
		return CMD_STATUS_INVALID_ARG;
	}

	if (period == 0) {
		CMD_ERR("invalid period %u\n", period);
		return CMD_STATUS_INVALID_ARG;
	}

	param.cfg = HAL_TIMER_MakeInitCfg(mode, clk_src, clk_prescaler);
	param.period = period;
	param.isEnableIRQ = 1;
	param.callback = cmd_timer_callback;
	param.arg = (void *)id;
	if (HAL_TIMER_Init((TIMER_ID)id, &param) != HAL_OK) {
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

/*
 * drv timer <action> i=<id>
 */
static enum cmd_status cmd_timer_action_exec(char *cmd, enum cmd_timer_action action)
{
	uint32_t id, tmp;
	int32_t cnt;
	enum cmd_status status;

	cnt = cmd_sscanf(cmd, "i=%u", &id);
	if (cnt != 1) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= TIMER_NUM) {
		CMD_ERR("invalid id %d\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	status = CMD_STATUS_OK;
	switch (action) {
	case CMD_TIMER_ACTION_DECONFIG:
		HAL_TIMER_DeInit((TIMER_ID)id);
		break;
	case CMD_TIMER_ACTION_START:
		HAL_TIMER_Start((TIMER_ID)id);
		break;
	case CMD_TIMER_ACTION_STOP:
		HAL_TIMER_Stop((TIMER_ID)id);
		break;
	case CMD_TIMER_ACTION_PAUSE:
		HAL_TIMER_Pause((TIMER_ID)id);
		break;
	case CMD_TIMER_ACTION_CONTINUE:
		HAL_TIMER_Continue((TIMER_ID)id);
		break;
	case CMD_TIMER_ACTION_VALUE:
		tmp = HAL_TIMER_GetCurrentValue((TIMER_ID)id);
		cmd_write_respond(CMD_STATUS_OK, "value=%u", tmp);
		status = CMD_STATUS_ACKED;
		break;
	default:
		status = CMD_STATUS_INVALID_ARG;
		break;
	}
	return status;
}

/*
 * drv timer deconfig i=<id>
 */
static enum cmd_status cmd_timer_deconfig_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_DECONFIG);
}

/*
 * drv timer start i=<id>
 */
static enum cmd_status cmd_timer_start_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_START);
}

/*
 * drv timer stop i=<id>
 */
static enum cmd_status cmd_timer_stop_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_STOP);
}

/*
 * drv timer pause i=<id>
 */
static enum cmd_status cmd_timer_pause_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_PAUSE);
}

/*
 * drv timer continue i=<id>
 */
static enum cmd_status cmd_timer_continue_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_CONTINUE);
}

/*
 * drv timer value i=<id>
 */
static enum cmd_status cmd_timer_value_exec(char *cmd)
{
	return cmd_timer_action_exec(cmd, CMD_TIMER_ACTION_VALUE);
}


static struct cmd_data g_timer_cmds[] = {
	{ "config",		cmd_timer_config_exec },
	{ "deconfig", 	cmd_timer_deconfig_exec },
	{ "start",		cmd_timer_start_exec },
	{ "stop",		cmd_timer_stop_exec },
	{ "pause",		cmd_timer_pause_exec },
	{ "continue",	cmd_timer_continue_exec },
	{ "value",		cmd_timer_value_exec },
};

enum cmd_status cmd_timer_exec(char *cmd)
{
	return cmd_exec(cmd, g_timer_cmds, cmd_nitems(g_timer_cmds));
}
