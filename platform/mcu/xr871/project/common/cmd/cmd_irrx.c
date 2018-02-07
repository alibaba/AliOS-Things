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
#include "cmd_irrx.h"
#include "driver/chip/hal_irrx.h"

enum cmd_irrx_action {
	CMD_IRRX_ACTION_DECONFIG,
	CMD_IRRX_ACTION_VALUE,
};

static uint32_t irrx_addr, irrx_key;
static IRRX_HandleTypeDef *irrx;

static void irrx_rxcplt_callback(uint32_t addr, uint32_t key)
{
	irrx_addr = addr;
	irrx_key = key;

	printf("received ir code addr:0x%02x key:0x%02x\n", addr, key);
}

/*
 * drv irrx config p=<PulsePolarity>
 */
static enum cmd_status cmd_irrx_config_exec(char *cmd)
{
	IRRX_InitTypeDef irrx_param;
	int32_t cnt;
	char PulsePolarity[8];

	cnt = cmd_sscanf(cmd, "p=%6s", PulsePolarity);
	if (cnt != 1) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(PulsePolarity, "none") == 0) {
		irrx_param.PulsePolariyInvert = IRRX_RPPI_NONE;
	} else if (cmd_strcmp(PulsePolarity, "invert") == 0) {
		irrx_param.PulsePolariyInvert = IRRX_RPPI_INVERT;
	} else {
		CMD_ERR("invalid PulsePolarity:%s\n", PulsePolarity);
		return CMD_STATUS_INVALID_ARG;
	}

	irrx_param.rxCpltCallback = &irrx_rxcplt_callback;

	irrx = HAL_IRRX_Init(&irrx_param);

	return CMD_STATUS_OK;
}

/*
 * drv irrx <action>
 */
static enum cmd_status cmd_irrx_action_exec(char *cmd, enum cmd_irrx_action action)
{
	enum cmd_status status;

	status = CMD_STATUS_OK;
	switch (action) {
	case CMD_IRRX_ACTION_DECONFIG:
		HAL_IRRX_DeInit(irrx);
		break;
	case CMD_IRRX_ACTION_VALUE:
		cmd_write_respond(CMD_STATUS_OK, "addr=0x%02x key=0x%02x", irrx_addr, irrx_key);
		status = CMD_STATUS_ACKED;
		break;
	default:
		status = CMD_STATUS_INVALID_ARG;
		break;
	}
	return status;
}

/*
 * drv irrx deconfig
 */
static enum cmd_status cmd_irrx_deconfig_exec(char *cmd)
{
	return cmd_irrx_action_exec(cmd, CMD_IRRX_ACTION_DECONFIG);
}

/*
 * drv irrx value
 */
static enum cmd_status cmd_irrx_value_exec(char *cmd)
{
	return cmd_irrx_action_exec(cmd, CMD_IRRX_ACTION_VALUE);
}

static struct cmd_data g_irrx_cmds[] = {
	{ "config",     cmd_irrx_config_exec },
	{ "deconfig",   cmd_irrx_deconfig_exec },
	{ "value",      cmd_irrx_value_exec },
};

enum cmd_status cmd_irrx_exec(char *cmd)
{
	return cmd_exec(cmd, g_irrx_cmds, cmd_nitems(g_irrx_cmds));
}
