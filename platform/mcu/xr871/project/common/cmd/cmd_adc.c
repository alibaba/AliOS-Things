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
#include "cmd_adc.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_adc.h"

#define CMD_ADC_TEST_DBG	0

static uint8_t cmd_adc_pass;

static struct cmd_adc_test_priv{
	uint8_t		enable;
	uint8_t		irq_mode;
	uint32_t	low_value;
	uint32_t	high_value;
	uint32_t	voltage;
	uint32_t	deviation;
} priv[ADC_CHANNEL_NUM];

static enum cmd_status cmd_adc_output_check(uint32_t data, uint32_t voltage,
											uint32_t deviation)
{
	uint32_t voltage_low, voltage_high;

	voltage_low = (voltage < deviation) ? 0 : (voltage - deviation);
	voltage_high = ((voltage + deviation) > 2500) ? 2500 : (voltage + deviation);

	voltage_low = voltage_low * 4096 / 2500;
	voltage_high = voltage_high * 4096 / 2500;

	if ((data >= voltage_low) && (data <= voltage_high)) {
#if CMD_ADC_TEST_DBG
		CMD_DBG("ADC: data = %u, voltage_low = %u, voltage_high = %u\n",
				data, voltage_low, voltage_high);
#endif /* CMD_ADC_TEST_DBG */
		return CMD_STATUS_OK;
	} else {
		CMD_ERR("ADC: data = %u, voltage_low = %u, voltage_high = %u\n",
				data, voltage_low, voltage_high);
		return CMD_STATUS_FAIL;
	}
}

static void cmd_adc_irq_cb(void *arg)
{
	uint32_t channel, data;
	ADC_IRQState irq_state;

	for (channel = ADC_CHANNEL_0; channel < ADC_CHANNEL_NUM; channel++) {
		if (priv[channel].enable == 0)
			continue;

		data		= HAL_ADC_GetValue((ADC_Channel)channel);
		irq_state	= HAL_ADC_GetIRQState((ADC_Channel)channel);

#if CMD_ADC_TEST_DBG
		CMD_DBG("channel = %u, irq_mode = %u, irq_state = %d, low_value = %u, high_value = %u, data = %u\n",
				channel, priv[channel].irq_mode, irq_state, priv[channel].low_value,
				priv[channel].high_value, data);
#endif /* CMD_ADC_TEST_DBG */

		if (CMD_STATUS_OK != cmd_adc_output_check(data, priv[channel].voltage, priv[channel].deviation))
			cmd_adc_pass = 0;

		switch (priv[channel].irq_mode) {
		case ADC_IRQ_NONE:
			cmd_adc_pass = 0;
			CMD_ERR("irq_mode: ADC_IRQ_NONE, irq_state = %d\n", irq_state);
			break;
		case ADC_IRQ_DATA:
			if (irq_state != ADC_DATA_IRQ) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_DATA, irq_state = %d\n", irq_state);
			}
			break;
		case ADC_IRQ_LOW:
			if ((irq_state != ADC_LOW_IRQ) || (data > priv[channel].low_value)) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_LOW, irq_state = %d, low_value = %u, data = %u\n",
						irq_state, priv[channel].low_value, data);
			}
			break;
		case ADC_IRQ_HIGH:
			if ((irq_state != ADC_HIGH_IRQ) || (data < priv[channel].high_value)) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_HIGH, irq_state = %d, high_value = %u, data = %u\n",
						irq_state, priv[channel].high_value, data);
			}
			break;
		case ADC_IRQ_LOW_DATA:
			if (irq_state == ADC_LOW_DATA_IRQ) {
				if (data > priv[channel].low_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, low_value = %u, data = %u\n",
							irq_state, priv[channel].low_value, data);
				}
			} else if (irq_state != ADC_DATA_IRQ) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_LOW_DATA, irq_state = %d\n", irq_state);
			}
			break;
		case ADC_IRQ_HIGH_DATA:
			if (irq_state == ADC_HIGH_DATA_IRQ) {
				if (data < priv[channel].high_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, high_value = %u, data = %u\n",
							irq_state, priv[channel].high_value, data);
				}
			} else if (irq_state != ADC_DATA_IRQ) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_HIGH_DATA, irq_state = %d\n", irq_state);
			}
			break;
		case ADC_IRQ_LOW_HIGH:
			if (irq_state == ADC_LOW_IRQ) {
				if (data > priv[channel].low_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, low_value = %u, data = %u\n",
							irq_state, priv[channel].low_value, data);
				}
			} else if (irq_state == ADC_HIGH_IRQ) {
				if (data < priv[channel].high_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, high_value = %u, data = %u\n",
							irq_state, priv[channel].high_value, data);
				}
			} else {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_LOW_HIGH, irq_state = %d\n", irq_state);
			}
			break;
		case ADC_IRQ_LOW_HIGH_DATA:
			if (irq_state == ADC_LOW_DATA_IRQ) {
				if (data > priv[channel].low_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, low_value = %u, data = %u\n",
							irq_state, priv[channel].low_value, data);
				}
			} else if (irq_state == ADC_HIGH_DATA_IRQ) {
				if (data < priv[channel].high_value) {
					cmd_adc_pass = 0;
					CMD_ERR("irq_state = %d, high_value = %u, data = %u\n",
							irq_state, priv[channel].high_value, data);
				}
			} else if (irq_state != ADC_DATA_IRQ) {
				cmd_adc_pass = 0;
				CMD_ERR("irq_mode: ADC_IRQ_LOW_HIGH_DATA, irq_state = %d\n", irq_state);
			}
			break;
		default:
			cmd_adc_pass = 0;
			CMD_ERR("channel = %u, irq_mode = %u\n", channel, priv[channel].irq_mode);
		}
	}

	return;
}

static enum cmd_status cmd_adc_init_exec(char *cmd)
{
	int cnt;
	uint32_t freq;
	uint32_t delay;
	ADC_InitParam adc_param;
	HAL_Status hal_status;

	cnt = cmd_sscanf(cmd, "f=%u d=%u", &freq, &delay);
	if (cnt != 2) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((freq < 1000) || (freq > 1000000)) {
		CMD_ERR("invalid freq %u\n", freq);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((delay >> 8) != 0) {
		CMD_ERR("invalid delay %u\n", delay);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_memset(priv, 0, ADC_CHANNEL_NUM * sizeof(struct cmd_adc_test_priv));

	adc_param.freq		= freq;
	adc_param.delay		= delay;

	hal_status = HAL_ADC_Init(&adc_param);
	if (hal_status == HAL_OK) {
		return CMD_STATUS_OK;
	} else {
		CMD_ERR("HAL_ADC_Init return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}
}

static enum cmd_status cmd_adc_deinit_exec(char *cmd)
{
	HAL_Status hal_status;

	hal_status = HAL_ADC_DeInit();
	if (hal_status == HAL_OK) {
		return CMD_STATUS_OK;
	} else {
		CMD_ERR("HAL_ADC_DeInit return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}
}

static enum cmd_status cmd_adc_conv_polling_exec(char *cmd)
{
	int cnt;
	uint32_t channel, voltage, deviation;
	uint32_t data;
	HAL_Status hal_status;

	cnt = cmd_sscanf(cmd, "c=%u v=%u d=%u", &channel, &voltage, &deviation);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (channel >= ADC_CHANNEL_NUM) {
		CMD_ERR("invalid channel %u\n", channel);
		return CMD_STATUS_INVALID_ARG;
	}

	if (voltage > 2500) {
		CMD_ERR("invalid voltage %u\n", voltage);
		return CMD_STATUS_INVALID_ARG;
	}

	if (deviation > 2500) {
		CMD_ERR("invalid deviation %u\n", deviation);
		return CMD_STATUS_INVALID_ARG;
	}

	hal_status = HAL_ADC_Conv_Polling((ADC_Channel)channel, &data, 10000);
	if (hal_status != HAL_OK) {
		CMD_ERR("HAL_ADC_Conv_Polling return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}

	return cmd_adc_output_check(data, voltage, deviation);
}

static enum cmd_status cmd_adc_config_exec(char *cmd)
{
	int cnt;
	uint32_t channel, enable, irq_mode, low_value, high_value, voltage, deviation;
	HAL_Status hal_status;

	cnt = cmd_sscanf(cmd, "c=%u e=%u i=%u l=%u h=%u v=%u d=%u",
					 &channel, &enable, &irq_mode, &low_value,
					 &high_value, &voltage, &deviation);
	if (cnt != 7) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (channel >= ADC_CHANNEL_NUM) {
		CMD_ERR("invalid channel %u\n", channel);
		return CMD_STATUS_INVALID_ARG;
	}

	if (enable > 1) {
		CMD_ERR("invalid enable %u\n", enable);
		return CMD_STATUS_INVALID_ARG;
	}

	if (irq_mode > 7) {
		CMD_ERR("invalid irq_mode %u\n", irq_mode);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((low_value >> 12) != 0) {
		CMD_ERR("invalid low_value %u\n", low_value);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((high_value >> 12) != 0) {
		CMD_ERR("invalid high_value %u\n", high_value);
		return CMD_STATUS_INVALID_ARG;
	}

	if (voltage > 2500) {
		CMD_ERR("invalid voltage %u\n", voltage);
		return CMD_STATUS_INVALID_ARG;
	}

	if (deviation > 2500) {
		CMD_ERR("invalid deviation %u\n", deviation);
		return CMD_STATUS_INVALID_ARG;
	}

	hal_status = HAL_ADC_ConfigChannel((ADC_Channel)channel, (ADC_Select)enable,
									   (ADC_IRQMode)irq_mode, low_value, high_value);
	if (hal_status != HAL_OK) {
		CMD_ERR("HAL_ADC_ConfigChannel return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}

	priv[channel].enable		= (uint8_t)enable;
	priv[channel].irq_mode		= (uint8_t)irq_mode;
	priv[channel].low_value		= low_value;
	priv[channel].high_value	= high_value;
	priv[channel].voltage		= voltage;
	priv[channel].deviation		= deviation;

#if CMD_ADC_TEST_DBG
	CMD_DBG("ADC config: channel = %u, enable = %u, irq_mode = %u, low_value = %u, high_value = %u, voltage = %u, deviation = %u\n",
			channel, enable, irq_mode, low_value, high_value, voltage, deviation);
#endif /* CMD_ADC_TEST_DBG */

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_adc_conv_it_start_exec(char *cmd)
{
	ADC_Channel chan;
	HAL_Status hal_status;

	cmd_adc_pass = 1;

	for (chan = ADC_CHANNEL_0; chan < ADC_CHANNEL_NUM; chan++) {
		hal_status = HAL_ADC_EnableIRQCallback(chan, cmd_adc_irq_cb, NULL);
		if (hal_status != HAL_OK) {
			CMD_ERR("HAL_ADC_EnableIRQCallback return: hal_status = %d\n", hal_status);
			return CMD_STATUS_FAIL;
		}
	}

	hal_status = HAL_ADC_Start_Conv_IT();
	if (hal_status != HAL_OK) {
		CMD_ERR("HAL_ADC_Start_Conv_IT return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_adc_conv_it_stop_exec(char *cmd)
{
	ADC_Channel chan;
	HAL_Status hal_status;

	hal_status = HAL_ADC_Stop_Conv_IT();
	if (hal_status != HAL_OK) {
		CMD_ERR("HAL_ADC_Stop_Conv_IT return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}

	for (chan = ADC_CHANNEL_0; chan < ADC_CHANNEL_NUM; chan++) {
		hal_status = HAL_ADC_DisableIRQCallback(chan);
		if (hal_status != HAL_OK) {
			CMD_ERR("HAL_ADC_DisableIRQCallback return: hal_status = %d\n", hal_status);
			return CMD_STATUS_FAIL;
		}
	}

	if (cmd_adc_pass == 1)
		return CMD_STATUS_OK;
	else
		return CMD_STATUS_FAIL;
}

static struct cmd2_data g_adc_cmds[] = {
	{ "init ",			5,	cmd_adc_init_exec },
	{ "deinit",			6,	cmd_adc_deinit_exec },
	{ "conv-polling ",	13,	cmd_adc_conv_polling_exec },
	{ "config ",		7,	cmd_adc_config_exec },
	{ "conv-it-start",	13,	cmd_adc_conv_it_start_exec },
	{ "conv-it-stop",	12,	cmd_adc_conv_it_stop_exec },
};

enum cmd_status cmd_adc_exec(char *cmd)
{
	return cmd2_exec(cmd, g_adc_cmds, cmd_nitems(g_adc_cmds));
}
