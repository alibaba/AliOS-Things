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
#include "cmd_gpio.h"
#include "driver/chip/hal_gpio.h"

/*
 * drv gpio config <port> <pin> <mode> <pull> <level>
 * eg. drv gpio config a 6 1 1 1
 */
static enum cmd_status cmd_gpio_config_exec(char *cmd)
{
	int32_t cnt;
	char port;
	uint32_t pin, mode, pull, level;
	GPIO_InitParam param;

	cnt = cmd_sscanf(cmd, "%c %d %d %d %d", &port, &pin, &mode, &pull, &level);
	if (cnt != 5 || pull > GPIO_CTRL_PULL_MAX) {
		CMD_ERR("err cmd:%s, expect: <Port> <Pin> <Mode> <Pull> <Level>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (port == 'A' || port == 'a')
		port = GPIO_PORT_A;
	else if (port == 'B' || port == 'b')
		port = GPIO_PORT_B;
	else
		return CMD_STATUS_INVALID_ARG;
	param.mode = mode;
	param.driving = level;
	param.pull = pull;
	HAL_GPIO_Init(port, pin, &param);

	return CMD_STATUS_OK;
}

/*
 * drv gpio deconfig <port> <pin>
 * eg. drv gpio deconfig a 6
 */
static enum cmd_status cmd_gpio_deconfig_exec(char *cmd)
{
	int32_t cnt;
	char port;
	uint32_t pin;

	cnt = cmd_sscanf(cmd, "%c %d", &port, &pin);
	if (cnt != 2) {
		CMD_ERR("err cmd:%s, expect: p=<Port_Num> m=<Mode> p=<Pull>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (port == 'A' || port == 'a')
		port = GPIO_PORT_A;
	else if (port == 'B' || port == 'b')
		port = GPIO_PORT_B;
	else
		return CMD_STATUS_INVALID_ARG;

	HAL_GPIO_DeInit(port, pin);

	return CMD_STATUS_OK;
}

/*
 * drv gpio read <port> <pin>
 * eg. drv gpio read a 6
 */
static enum cmd_status cmd_gpio_read_exec(char *cmd)
{
	enum cmd_status ret = CMD_STATUS_OK;
	int32_t cnt;
	char port;
	uint32_t pin, state;

	cnt = cmd_sscanf(cmd, "%c %d", &port, &pin);

	if (port == 'A' || port == 'a')
		port = GPIO_PORT_A;
	else if (port == 'B' || port == 'b')
		port = GPIO_PORT_B;
	else
		ret = CMD_STATUS_INVALID_ARG;

	if (cnt != 2 || ret == CMD_STATUS_INVALID_ARG) {
		CMD_ERR("err cmd:%s, expect: p=<Port_Num> m=<Mode> p=<Pull>\n", cmd);
		return ret;
	}

	state = HAL_GPIO_ReadPin(port, pin);
	CMD_DBG("read gpio value:%d\n", state);

	return CMD_STATUS_OK;
}

/*
 * drv gpio write <port> <pin> <value>
 * eg. drv gpio write a 6 0
 */
static enum cmd_status cmd_gpio_write_exec(char *cmd)
{
	int32_t cnt;
	char port;
	uint32_t pin, state;

	cnt = cmd_sscanf(cmd, "%c %d %d", &port, &pin, &state);
	if (cnt != 3) {
		CMD_ERR("err cmd:%s, expect: p=<Port_Num> m=<Mode> p=<Pull>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (port == 'A' || port == 'a')
		port = GPIO_PORT_A;
	else if (port == 'B' || port == 'b')
		port = GPIO_PORT_B;
	else
		return CMD_STATUS_INVALID_ARG;

	HAL_GPIO_WritePin(port, pin, state);

	return CMD_STATUS_OK;
}

static struct cmd_data g_gpio_cmds[] = {
	{ "config",	cmd_gpio_config_exec },
	{ "deconfig", 	cmd_gpio_deconfig_exec },
	{ "read",	cmd_gpio_read_exec },
	{ "write",	cmd_gpio_write_exec },
};

enum cmd_status cmd_gpio_exec(char *cmd)
{
	return cmd_exec(cmd, g_gpio_cmds, cmd_nitems(g_gpio_cmds));
}
