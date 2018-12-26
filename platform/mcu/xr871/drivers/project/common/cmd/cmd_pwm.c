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
#include "cmd_pwm.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_pwm.h"

typedef enum {
	PWM_COMPLE,
	PWM_PLUSE,
	PWM_CYCLE,
	PWM_CAPTURE,
	PWM_DEADZONE,
	PWM_MODENUM,
}Cmd_PwmMode;

typedef struct {
	uint32_t high_time;
	uint32_t low_time;
	uint16_t num;
	uint16_t count;
	uint8_t d_value;
	uint8_t d_hz;
	uint8_t input_ch;
} Cmd_Capinfo;

static Cmd_Capinfo private_cap[8];

static Cmd_PwmMode cmd_pwm_mode_analytic(char *data)
{
	if (cmd_strcmp(data, "comple") == 0) {
		return PWM_COMPLE;
	} else if (cmd_strcmp(data, "pluse") == 0) {
		return PWM_PLUSE;
	} else if (cmd_strcmp(data, "cycle") == 0) {
		return PWM_CYCLE;
	} else if (cmd_strcmp(data, "capture") == 0) {
		return PWM_CAPTURE;
	} else if (cmd_strcmp(data, "deadzone") == 0) {
	 	return PWM_DEADZONE;
	}
	return PWM_MODENUM;
}

typedef struct {
	PWM_GROUP_ID group;
	PWM_CH_ID ch;
}PWM_IoInfo;

static int cmd_pwm_init(Cmd_PwmMode mode, PWM_IoInfo info, uint32_t hz)
{
	HAL_Status ret = HAL_ERROR;
	PWM_ClkParam clk_cfg;
	PWM_CompInitParam comp_cfg;
	PWM_ChInitParam ch_cfg;
	int cycle = 0;

	clk_cfg.clk = PWM_CLK_HOSC;
	clk_cfg.div =  PWM_SRC_CLK_DIV_1;

	ret = HAL_PWM_GroupClkCfg(info.group, &clk_cfg);
	if (ret != HAL_OK)
			return -1;

	switch(mode) {
		case PWM_COMPLE:
			comp_cfg.hz = hz;
			comp_cfg.polarity = PWM_HIGHLEVE;

			cycle = HAL_PWM_ComplementaryInit(info.group, &comp_cfg);
			if (cycle == -1)
				return -1;

			ret = HAL_PWM_ComplementarySetDutyRatio(info.group, cycle / 4);
			if (ret != HAL_OK)
				return -1;

			break;
		case PWM_CYCLE:
			ch_cfg.hz = hz;
			ch_cfg.mode = PWM_CYCLE_MODE;
			ch_cfg.polarity = PWM_HIGHLEVE;

			cycle = HAL_PWM_ChInit(info.ch, &ch_cfg);
			if (cycle == -1)
				return -1;

			ret = HAL_PWM_ChSetDutyRatio(info.ch, cycle / 2);
			if (ret != HAL_OK)
				return -1;

			break;
		case PWM_PLUSE:
			ch_cfg.hz = hz;
			ch_cfg.mode = PWM_PLUSE_MODE;
			ch_cfg.polarity = PWM_HIGHLEVE;

			cycle = HAL_PWM_ChInit(info.ch, &ch_cfg);
			if (cycle == -1)
				return -1;
			break;
		case PWM_CAPTURE:
			ch_cfg.hz = hz;
			ch_cfg.mode = PWM_CAPTURE_MODE;
			ch_cfg.polarity = PWM_HIGHLEVE;

			cycle = HAL_PWM_ChInit(info.ch, &ch_cfg);
			if (cycle == -1)
				return -1;

			break;
		default :
			CMD_DBG("invalid PWM mode\n");
			return -1;
	}
	return 0;
}

static PWM_IoInfo cmd_pwm_channel_analytic(Cmd_PwmMode mode, uint32_t channel)
{
	PWM_IoInfo info;
	if (mode == PWM_COMPLE || mode == PWM_DEADZONE) {
		if (channel > 3) {
			info.ch = PWM_CH_NULL;
			info.group = PWM_GROUP_NULL;
			return info;
		}
		info.group = channel;
		info.ch = PWM_CH_NULL;
		return info;
	} else if (channel > 7) {
		info.ch = PWM_CH_NULL;
		info.group = PWM_GROUP_NULL;
		return info;
	}

	info.ch = channel;
	info.group = channel / 2;

	return info;
}

static enum cmd_status cmd_pwm_config_exec(char *cmd)
{
	uint32_t hz;
	uint32_t channel;
	char mode_char[8];
	int cnt;

	cnt = cmd_sscanf(cmd, "c=%u m=%s h=%u", &channel, mode_char, &hz);

	if (cnt != 3) {
		return CMD_STATUS_INVALID_ARG;
	}

	Cmd_PwmMode mode = cmd_pwm_mode_analytic(mode_char);
	if (mode >= PWM_MODENUM) {
		CMD_ERR("invalid pwm mode %u\n", mode);
		return CMD_STATUS_INVALID_ARG;
	}
	if (hz < 2 || hz > 240000) {
		CMD_ERR("pwm hz out of range %u\n", hz);
		return CMD_STATUS_FAIL;
	}

	PWM_IoInfo info = cmd_pwm_channel_analytic(mode, channel);

	if (info.group >= PWM_GROUP_NULL)
		return CMD_STATUS_INVALID_ARG;

	if (cmd_pwm_init(mode, info, hz) == -1)
		return CMD_STATUS_FAIL;

	return CMD_STATUS_OK;
}

enum cmd_status  cmd_pwm_deinit_exec(char *cmd)
{
	int cnt = 0;
	uint32_t channel;
	char mode_char[8];
	HAL_Status ret = HAL_ERROR;

	cnt = cmd_sscanf(cmd, "c=%u m=%s", &channel, mode_char);
	if (cnt != 2)
		return CMD_STATUS_INVALID_ARG;

	Cmd_PwmMode mode = cmd_pwm_mode_analytic(mode_char);
	if (mode >= PWM_MODENUM) {
		CMD_ERR("invalid pwm mode %u\n", mode);
		return CMD_STATUS_INVALID_ARG;
	}

	PWM_IoInfo info = cmd_pwm_channel_analytic(mode, channel);
	if (info.group == PWM_GROUP_NULL) {
		CMD_ERR("invalid pwm channel\n");
		return CMD_STATUS_INVALID_ARG;
	}

	if (mode == PWM_COMPLE) {
		ret = HAL_PWM_ComplementaryDeInit(info.group);
		if (ret != HAL_OK)
			return CMD_STATUS_FAIL;
	} else {
		ret = HAL_PWM_ChDeinit(info.ch);
		if (ret != HAL_OK)
			return CMD_STATUS_FAIL;
	}
	return CMD_STATUS_OK;
}

enum cmd_status  cmd_pwm_start_exec(char *cmd)
{
	HAL_Status ret;
	char mode_char[8];
	uint32_t channel;
	int cnt;
	PWM_IrqParam irq_cfg;

	cnt = cmd_sscanf(cmd, "c=%u m=%s", &channel, mode_char);
	if (cnt != 2)
		return CMD_STATUS_INVALID_ARG;

	Cmd_PwmMode mode = cmd_pwm_mode_analytic(mode_char);
	if (mode >= PWM_MODENUM) {
		CMD_ERR("invalid pwm mode %u\n", mode);
		return CMD_STATUS_INVALID_ARG;
	}

	PWM_IoInfo info = cmd_pwm_channel_analytic(mode, channel);
	if (info.group == PWM_GROUP_NULL) {
		CMD_DBG("invalid pwm channel\n");
		return CMD_STATUS_INVALID_ARG;
	}

	switch (mode) {
		case PWM_COMPLE:
			ret = HAL_PWM_EnableComplementary(info.group, 1);
			if (ret != HAL_OK) {
				CMD_ERR("Enable comple error\n");
				return CMD_STATUS_FAIL;
			}
			break;
		case PWM_PLUSE:
			ret = HAL_PWM_EnableCh(info.ch, PWM_PLUSE_MODE, 1);
			if (ret != HAL_OK) {
				CMD_ERR("Enable channel error\n");
				return CMD_STATUS_FAIL;
			}

			ret = HAL_PWM_OutputPluse(info.ch);
			if (ret != HAL_OK) {
				CMD_ERR("start pluse fail, it's busy\n");
				return CMD_STATUS_FAIL;
			}
			break;
		case PWM_CYCLE:
			ret = HAL_PWM_EnableCh(info.ch, PWM_CYCLE_MODE, 1);
			if (ret != HAL_OK) {
				CMD_ERR("Enable channel error\n");
				return CMD_STATUS_FAIL;
			}
			break;
		case PWM_CAPTURE:
			ret = HAL_PWM_EnableCh(info.ch, PWM_CAPTURE_MODE, 1);
			if (ret != HAL_OK) {
				CMD_ERR("Enable channel error\n");
				return CMD_STATUS_FAIL;
			}

			irq_cfg.arg = NULL;
			irq_cfg.callback = NULL;
			irq_cfg.event = PWM_IRQ_BOTHEDGE;

			ret = HAL_PWM_EnableIRQ(info.ch, &irq_cfg);
			if (ret != HAL_OK) {
				CMD_ERR("Enable irq error\n");
				return CMD_STATUS_FAIL;
			}
			break;
		case PWM_DEADZONE:
			ret = HAL_PWM_EnableDeadZone(info.group, 1);
			if (ret != HAL_OK)
				CMD_ERR("Enable dead zone error\n");
				return CMD_STATUS_FAIL;

			break;
		default:
			CMD_DBG("invalid pwm mode %u\n", mode);
			return CMD_STATUS_INVALID_ARG;
	}
	return CMD_STATUS_OK;
}

enum cmd_status  cmd_pwm_set_exec(char *cmd)
{
	uint32_t value;
	uint32_t channel;
	char function[10];
	int cnt;
	PWM_IoInfo info;
	cnt = cmd_sscanf(cmd, "c=%u m=%s v=%u", &channel, function, &value);

	if (cnt != 3)
		return CMD_STATUS_INVALID_ARG;

	if (cmd_strcmp(function, "comple") == 0 ||
		cmd_strcmp(function, "deadzone") == 0 )
		info = cmd_pwm_channel_analytic(PWM_COMPLE, channel);
	else
		info = cmd_pwm_channel_analytic(PWM_CYCLE, channel);

	if (info.group == PWM_GROUP_NULL) {
		CMD_ERR("invalid pwm channel\n");
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(function, "deadzone") == 0) {
		if (value > 255) {
			CMD_ERR("deadzone value out of range\n");
			return CMD_STATUS_INVALID_ARG;
		}
		HAL_PWM_SetDeadZoneTime(info.group, value);
	} else if (cmd_strcmp(function, "comple") == 0) {
		__IO uint32_t *reg;

		if (value > 0) {
			info.ch = info.group * 2;

			reg = &PWM->CH_REG[info.ch].PPR;
			value = value *	((*reg & PWM_PPR_ENTIER_CYCLE) >> 16) / 10000;
			if (value <= 1)
				value = 2;
		}

		HAL_PWM_ComplementarySetDutyRatio(info.group, value);
	} else if (cmd_strcmp(function, "cycle") == 0) {
		__IO uint32_t *reg;

		if (value > 0) {
			reg = &PWM->CH_REG[info.ch].PPR;
			value = value *	((*reg & PWM_PPR_ENTIER_CYCLE) >> 16) / 10000;
			if (value <= 1)
				value = 2;
		}

		HAL_PWM_ChSetDutyRatio(info.ch, value);
	} else if (cmd_strcmp(function, "pluse") == 0) {
		__IO uint32_t *reg;

		if (value > 0) {
			reg = &PWM->CH_REG[info.ch].PPR;
			value = value *	((*reg & PWM_PPR_ENTIER_CYCLE) >> 16) / 10000;
			if (value <= 1)
				value = 2;
		}

		HAL_PWM_ChSetDutyRatio(info.ch, value);
	} else
		return CMD_STATUS_INVALID_ARG;

	return CMD_STATUS_OK;
}


static int PWM_DeadTime(uint8_t chGroup)
{

	if (chGroup >= PWM_GROUP_NUM)
		return -1;

	uint32_t p = PWM->PDZCR[chGroup];
	p &= PWM_CH_DZ_INV;
	return (p >> 8);
}

static int PWM_ReadActCycle(uint8_t ch)
{
	if (ch >= PWM_CH_NUM)
		return -1;
	int p = (int)PWM->CH_REG[ch].PPR;
	return p;
}

static PWM_Polarity _PWM_Polarity(uint8_t ch)
{
	if ((PWM->CH_REG[ch].PCR & PWM_PCR_ACT_STA) > 0)
		return PWM_HIGHLEVE;

	return PWM_LOWLEVE;

}

static Cmd_PwmMode PWM_ChMode(uint8_t ch)
{
	__IO uint32_t *reg = NULL;
	reg = &PWM->CH_REG[ch].PCR;

	if ((*reg & PWM_PCR_MODE) > 0)
		return PWM_PLUSE_MODE;

	return PWM_CYCLE_MODE;
}

int PWM_DeadZoneEnable(PWM_GROUP_ID chGroup)
{
	if (chGroup >= PWM_GROUP_NUM)
		return -1;

	return PWM->PDZCR[chGroup]&PWM_CH_DZ_EN;
}


#define CAPTURE_TASK_THREAD_STACK_SIZE	(2 * 1024)
static OS_Thread_t g_capture_thread_t;

void cmd_capture_task(void *arg)
{
	int i = 0;

	if (PWM->CIER == 0) {
		OS_ThreadDelete(&g_capture_thread_t);
		return;
	}

	PWM_CapResult result;

	while (PWM->CIER) {
		Cmd_Capinfo *info = &private_cap[i];

		if (info->count < info->num) {
			while (info->count < info->num) {
				if ( PWM_ChMode(info->input_ch) == PWM_PLUSE)
					result = HAL_PWM_CaptureResult(PWM_CAP_PLUSE, i);
				else
					result = HAL_PWM_CaptureResult(PWM_CAP_CYCLE, i);

				if (result.highLevelTime) {
					info->count += 1;
					if (info->count == 1)
						continue;

					info->high_time += result.highLevelTime;
					info->low_time += result.lowLevelTime;
				}
			}
		} else
			OS_MSleep(10);

		i ++;
		if (i >= 8)
		i = 0;
	}
	OS_ThreadDelete(&g_capture_thread_t);
}

enum cmd_status  cmd_pwm_get_exec(char *cmd)
{
	uint32_t channel;
	uint32_t src_signal;
	char function[8];
	uint32_t num = 0;
	uint32_t value_deviation = 0;
	uint32_t hz_deviation = 0;
	int cnt;

	cnt = cmd_sscanf(cmd, "c=%u m=%s n=%u input_ch=%u dv=%u dh=%u", &channel, function, &num,
		                                     &src_signal, &value_deviation, &hz_deviation);
	if (cnt != 6) {
		CMD_ERR("miss param, %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (num > 1000) {
		CMD_ERR("The n value out of range\n");
		return CMD_STATUS_INVALID_ARG;
	}

	if (value_deviation > 100 || hz_deviation > 100) {
		CMD_ERR("The value out of range\n");
		return CMD_STATUS_FAIL;
	}

	PWM_IoInfo info;
	info = cmd_pwm_channel_analytic(PWM_CYCLE, src_signal);
	if (info.group== PWM_GROUP_NULL) {
		CMD_ERR("invalid src_signal\n");
		return CMD_STATUS_INVALID_ARG;
	}

	info = cmd_pwm_channel_analytic(PWM_CAPTURE, channel);
	if (info.group== PWM_GROUP_NULL) {
		CMD_ERR("invalid ch \n");
		return CMD_STATUS_INVALID_ARG;
	}
	Cmd_Capinfo *cap_info = private_cap;

	cap_info[channel].num = num;
	cap_info[channel].count = 0;
	cap_info[channel].d_hz = hz_deviation;
	cap_info[channel].d_value = value_deviation;
	cap_info[channel].input_ch = src_signal;
	cap_info[channel].high_time = 0;
	cap_info[channel].low_time = 0;

	if (!OS_ThreadIsValid(&g_capture_thread_t)) {
		if (OS_ThreadCreate(&g_capture_thread_t,
	                    "",
	                    cmd_capture_task,
	                    NULL,
	                    OS_PRIORITY_ABOVE_NORMAL,
	                    CAPTURE_TASK_THREAD_STACK_SIZE) != OS_OK) {
			CMD_ERR("create sys ctrl task failed\n");
		}
	}

	return CMD_STATUS_OK;
}


typedef enum {
	CAP_ERROR,
	CAP_BUSY,
	CAP_OK,
}CAP_RESULT;

CAP_RESULT cmd_CapResult(uint8_t ch) {
	CAP_RESULT ret;

	uint16_t input_right_period = 0, input_right_h_time = 0;
	uint8_t deadzone_time = 0;
	Cmd_Capinfo *info = &private_cap[ch];

	if (PWM_DeadZoneEnable(info->input_ch/ 2))
		deadzone_time = PWM_DeadTime(info->input_ch / 2);

	if (info->count == info->num) {
		uint32_t cap_period = 0;
		uint32_t cap_h_time = 0;

		__IO uint32_t *reg =  &PWM->CH_REG[info->input_ch].PPR;

		input_right_period = (*reg & PWM_PPR_ENTIER_CYCLE) >> 16;
		input_right_h_time = PWM_ReadActCycle(info->input_ch);

		if (_PWM_Polarity(info->input_ch)== PWM_LOWLEVE)
			input_right_h_time = input_right_period- input_right_h_time;

		if (deadzone_time > 0) {
			input_right_h_time -= deadzone_time;
		}

		cap_h_time = info->high_time / (info->num - 1);
		cap_period = cap_h_time + info->low_time / (info->num - 1);

		if ( PWM_ChMode(info->input_ch) == PWM_CYCLE) {
			if (abs(cap_h_time - input_right_h_time) <= info->d_value &&
					abs(cap_period - input_right_period) <= info->d_hz)
				ret = CAP_OK;
			else
				ret = CAP_ERROR;
		} else {
			if (abs(cap_h_time - input_right_h_time) <= info->d_value)
				ret = CAP_OK;
			else
				ret = CAP_ERROR;
		}

		memset(info, 0, sizeof(Cmd_Capinfo));

		return ret;
	}

	return CAP_BUSY;
}


enum cmd_status  cmd_pwm_get_result_exec(char *cmd)
{
	uint32_t channel;
	int cnt;

	cnt = cmd_sscanf(cmd, "c=%u ", &channel);
	if (cnt != 1)
		return CMD_STATUS_INVALID_ARG;

	PWM_IoInfo info = cmd_pwm_channel_analytic(PWM_CAPTURE, channel);

	if (info.group== PWM_GROUP_NULL) {
		CMD_ERR("invalid pwm channel\n");
		return CMD_STATUS_INVALID_ARG;
	}

	CAP_RESULT ret = cmd_CapResult(channel);

	if (ret == CAP_BUSY) {
		CMD_ERR("PWM%d Capture BUSY\n", info.ch);
		return CMD_STATUS_ERROR_MIN;
	} else if (ret == CAP_ERROR) {
		CMD_ERR("PWM%d Capture ERROR\n",info.ch);
		return CMD_STATUS_FAIL;
	} else if (ret == CAP_OK) {
		return CMD_STATUS_OK;
	}

	return CMD_STATUS_OK;
}

enum cmd_status cmd_pwm_stop_exec(char *cmd)
{
	char mode_char[8];
	uint32_t channel;
	int cnt;

	cnt = cmd_sscanf(cmd, "c=%u m=%s", &channel, mode_char);
	if (cnt != 2)
		return CMD_STATUS_INVALID_ARG;

	Cmd_PwmMode mode = cmd_pwm_mode_analytic(mode_char);
	if (mode >= PWM_MODENUM) {
		CMD_ERR("invalid pwm mode %u\n", mode);
		return CMD_STATUS_INVALID_ARG;
	}

	PWM_IoInfo info = cmd_pwm_channel_analytic(mode, channel);
	if (info.group== PWM_GROUP_NULL) {
		CMD_ERR("invalid pwm channel\n");
		return CMD_STATUS_INVALID_ARG;
	}



	switch(mode) {
			case PWM_COMPLE:
				HAL_PWM_EnableComplementary(info.group, 0);
				HAL_PWM_SetDeadZoneTime(info.group, 0);
				HAL_PWM_EnableDeadZone(info.group, 0);
				break;
			case PWM_PLUSE:
				HAL_PWM_EnableCh(info.ch, PWM_PLUSE_MODE, 0);
				break;
			case PWM_CYCLE:
				HAL_PWM_EnableCh(info.ch, PWM_CYCLE_MODE, 0);
				break;
			case PWM_CAPTURE:
				HAL_PWM_EnableCh(info.ch, PWM_CAPTURE_MODE, 0);
				HAL_PWM_DisableIRQ(info.ch);
				break;
			case PWM_DEADZONE:
				HAL_PWM_SetDeadZoneTime(info.group, 0);
				HAL_PWM_EnableDeadZone(info.group, 0);
				break;
			default :
				CMD_ERR("invalid pwm mode\n");
				return CMD_STATUS_INVALID_ARG;
		}
	return CMD_STATUS_OK;
}

/*
 * driver commands
 */
static struct cmd_data g_pwm_cmds[] = {
	{ "deinit",	cmd_pwm_deinit_exec },
	{ "config",	cmd_pwm_config_exec },
	{ "start",	cmd_pwm_start_exec },
	{ "set",	cmd_pwm_set_exec },
	{ "get",	cmd_pwm_get_exec },
	{ "get_result", cmd_pwm_get_result_exec },
	{ "stop",	cmd_pwm_stop_exec },
};

enum cmd_status cmd_pwm_exec(char *cmd)
{
	return cmd_exec(cmd, g_pwm_cmds, cmd_nitems(g_pwm_cmds));
}

