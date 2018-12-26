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
#include "cmd_irtx.h"
#include "driver/chip/hal_clock.h"
#include "driver/chip/hal_irtx.h"
#include "driver/chip/ir_nec.h"

enum cmd_irtx_action {
	CMD_IRTX_ACTION_DECONFIG,
	CMD_IRTX_ACTION_SEND,
};

static IRTX_HandleTypeDef *irtx;

/*
 * drv irtx config p=<PulsePolarity> d=<ModulateDutyLevel> t=<SendModeType>
 *                 c=<CyclicalCnt> m=<ModulationEnable> r=<Protocol>
 */
static enum cmd_status cmd_irtx_config_exec(char *cmd)
{
	char PulsePolarity[8], ModulateDutyLevel[4], SendModeType[8], Modulation[4], Protocol[8];
	int32_t cnt;
	int32_t CyclicalCnt;
	IRTX_InitTypeDef irtx_param;

	cnt = cmd_sscanf(cmd, "p=%6s d=%2s t=%5s c=%u m=%3s r=%3s",
	                 PulsePolarity, ModulateDutyLevel, SendModeType, &CyclicalCnt, Modulation, Protocol);
	if (cnt != 6) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(PulsePolarity, "none") == 0) {
		irtx_param.PulsePolarity = IRTX_TPPI_NONE;
	} else if (cmd_strcmp(PulsePolarity, "invert") == 0) {
		irtx_param.PulsePolarity = IRTX_TPPI_INVERT;
	} else {
		CMD_ERR("invalid PulsePolarity:%s\n", PulsePolarity);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(ModulateDutyLevel, "t1") == 0) {
		irtx_param.ModulateDutyLevel = IRTX_DRMC_TIME_1;
	} else if (cmd_strcmp(ModulateDutyLevel, "t2") == 0) {
		irtx_param.ModulateDutyLevel = IRTX_DRMC_TIME_2;
	} else if (cmd_strcmp(ModulateDutyLevel, "t3") == 0) {
		irtx_param.ModulateDutyLevel = IRTX_DRMC_TIME_3;
	} else {
		CMD_ERR("invalid ModulateDutyLevel:%s\n", ModulateDutyLevel);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(SendModeType, "none") == 0) { /* IRTX_TTS_NONE for signal mode */
		irtx_param.SendModeType = IRTX_TTS_NONE;
	} else if (cmd_strcmp(SendModeType, "cycle") == 0) { /* IRTX_TTS_CYCLICAL for cycle mode */
		irtx_param.SendModeType = IRTX_TTS_CYCLICAL;
	} else {
		CMD_ERR("invalid SendModeType:%s\n", SendModeType);
		return CMD_STATUS_INVALID_ARG;
	}


	if (cmd_strcmp(Modulation, "dis") == 0) { /* IRTX_TTS_NONE for signal mode */
		irtx_param.InternalModulation = IRTX_IMS_DISABLE;
	} else if (cmd_strcmp(Modulation, "en") == 0) { /* IRTX_TTS_CYCLICAL for cycle mode */
		irtx_param.InternalModulation = IRTX_IMS_ENABLE;
	} else {
		CMD_ERR("invalid InternalModulation:%s\n", Modulation);
		return CMD_STATUS_INVALID_ARG;
	}

	irtx_param.CyclicalCnt = CyclicalCnt; /* send CyclicalCnt times if cycle mode */

	if (CyclicalCnt > 20) {
		CMD_ERR("invalid CyclicalCnt:%d\n", CyclicalCnt);
		return CMD_STATUS_INVALID_ARG;
	}
	if (cmd_strcmp(Protocol, "nec") == 0) {
#if defined (IR_CLK_32K_USED)
		irtx_param.IdleDurationCnt = IRTX_32K_NEC_IDC_VALUE;
#else
		uint32_t clk = HAL_GetHFClock();

		if (clk == HOSC_CLOCK_26M) {
			irtx_param.IdleDurationCnt = IRTX_26M_NEC_IDC_VALUE;
		} else if (clk == HOSC_CLOCK_24M) {
			irtx_param.IdleDurationCnt = IRTX_24M_NEC_IDC_VALUE;
		} else {
			CMD_ERR("%s unknow clk type(%d)!\n", __func__, clk);
		}
#endif
	} else {
		CMD_ERR("invalid protocol:%s\n", Protocol);
		return CMD_STATUS_INVALID_ARG;
	}

	irtx = HAL_IRTX_Init(&irtx_param);

	return CMD_STATUS_OK;
}

/*
 * drv irtx <action>
 */
static enum cmd_status cmd_irtx_action_exec(char *cmd, enum cmd_irtx_action action)
{
	enum cmd_status status;

	status = CMD_STATUS_OK;
	switch (action) {
	case CMD_IRTX_ACTION_DECONFIG:
		HAL_IRTX_DeInit(irtx);
		break;
	case CMD_IRTX_ACTION_SEND: {
		int32_t addr, key;
		int32_t cnt;

		cnt = cmd_sscanf(cmd, "a=0x%x k=0x%x", &addr, &key);
		if (cnt != 2) {
			return CMD_STATUS_INVALID_ARG;
		}
		HAL_IRTX_Transmit(irtx, IRTX_NEC_PROTO, IR_NEC_CODE(addr, key));
		break;
		}
	default:
		status = CMD_STATUS_INVALID_ARG;
		break;
	}
	return status;
}

/*
 * drv irtx deconfig
 */
static enum cmd_status cmd_irtx_deconfig_exec(char *cmd)
{
	return cmd_irtx_action_exec(cmd, CMD_IRTX_ACTION_DECONFIG);
}

/*
 * drv irtx send
 */
static enum cmd_status cmd_irtx_send_exec(char *cmd)
{
	return cmd_irtx_action_exec(cmd, CMD_IRTX_ACTION_SEND);
}

static struct cmd_data g_irtx_cmds[] = {
	{ "config",     cmd_irtx_config_exec },
	{ "deconfig",   cmd_irtx_deconfig_exec },
	{ "send",       cmd_irtx_send_exec },
};

enum cmd_status cmd_irtx_exec(char *cmd)
{
	return cmd_exec(cmd, g_irtx_cmds, cmd_nitems(g_irtx_cmds));
}
