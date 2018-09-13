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
#include "driver/chip/hal_clock.h"

enum CMD_CLOCK_TYPE {
	CMD_CLOCK_TYPE_HF = 0,
	CMD_CLOCK_TYPE_LF,
	CMD_CLOCK_TYPE_CPU,
	CMD_CLOCK_TYPE_DEV,
	CMD_CLOCK_TYPE_AHB1,
	CMD_CLOCK_TYPE_AHB2,
	CMD_CLOCK_TYPE_APB,
	CMD_CLOCK_TYPE_NUM
};

/* index by enum CMD_CLOCK_TYPE */
static const char *cmd_clock_type_str[CMD_CLOCK_TYPE_NUM] = {
	"LF", "HF", "CPU", "DEV", "AHB1", "AHB2", "APB"
};

static uint32_t cmd_clock_get(enum CMD_CLOCK_TYPE type)
{
	uint32_t clock;

	switch (type) {
	case CMD_CLOCK_TYPE_HF:
		clock = HAL_GetLFClock();
		break;
	case CMD_CLOCK_TYPE_LF:
		clock = HAL_GetHFClock();
		break;
	case CMD_CLOCK_TYPE_CPU:
		clock = HAL_GetCPUClock();
		break;
	case CMD_CLOCK_TYPE_DEV:
		clock = HAL_GetDevClock();
		break;
	case CMD_CLOCK_TYPE_AHB1:
		clock = HAL_GetAHB1Clock();
		break;
	case CMD_CLOCK_TYPE_AHB2:
		clock = HAL_GetAHB2Clock();
		break;
	case CMD_CLOCK_TYPE_APB:
		clock = HAL_CCM_BusGetAPBClock();
		break;
	default:
		clock = 0;
		break;
	}
	return clock;
}

/*
 * drv clock <type>
 */
enum cmd_status cmd_clock_exec(char *cmd)
{
	int type;
	uint32_t clock = 0;

	for (type = 0; type < CMD_CLOCK_TYPE_NUM; ++type) {
		if (cmd_strcmp(cmd, cmd_clock_type_str[type]) == 0) {
			clock = cmd_clock_get((enum CMD_CLOCK_TYPE)type);
			break;
		}
	}

	if (clock != 0) {
		cmd_write_respond(CMD_STATUS_OK, "%d Hz (%s clock)", clock, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_INVALID_ARG;
}
