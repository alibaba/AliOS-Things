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
#include "cmd_uart.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_uart.h"


typedef int32_t (*uart_receive_func)(UART_ID uartID, uint8_t *buf, int32_t size, uint32_t msec);
typedef int32_t (*uart_transmit_func)(UART_ID uartID, uint8_t *buf, int32_t size);

#define CMD_UART_TRANSFER_BUF_SIZE			2048
#define CMD_UART_QUEUE_LEN					1
#define CMD_UART_TRANSFER_THREAD_STACK_SIZE	(1 * 1024)

struct cmd_uart_msg {
	int32_t len;
	uint32_t timeout;
};

enum cmd_uart_transfer_mode {
	CMD_UART_TRANSFER_MODE_IT	= 0,
	CMD_UART_TRANSFER_MODE_DMA,
	CMD_UART_TRANSFER_MODE_POLL,
};

struct cmd_uart_priv {
	UART_ID						id;
	enum cmd_uart_transfer_mode	mode;
	OS_Queue_t					queue;
	OS_Thread_t					thread;
};

struct cmd_uart_priv g_cmd_uart_priv;

UART_DataBits uart_data_bits[4] = {
	UART_DATA_BITS_5, UART_DATA_BITS_6, UART_DATA_BITS_7, UART_DATA_BITS_8
};

UART_StopBits uart_stop_bits[2] = { UART_STOP_BITS_1, UART_STOP_BITS_2 };

/*
 * drv uart config i=<id> b=<baud-rate> d=<data-bits> p=<parity> s=<stop-bits> f=<flow-ctrl>
 */
static enum cmd_status cmd_uart_config_exec(char *cmd)
{
	uint32_t id, baud_rate, data_bits, stop_bits;
	char parity[8];
	char flow_ctrl[8];
	int cnt;
	UART_Parity uart_parity;
	UART_InitParam uart_param;

	cnt = cmd_sscanf(cmd, "i=%u b=%u d=%u p=%7s s=%u f=%7s", &id, &baud_rate,
	                 &data_bits, parity, &stop_bits, flow_ctrl);
	if (cnt != 6) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= UART_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if (baud_rate < 110 || baud_rate > 921600) {
		CMD_ERR("invalid baud rate %u\n", baud_rate);
		return CMD_STATUS_INVALID_ARG;
	}

	if (data_bits < 5 || data_bits > 8) {
		CMD_ERR("invalid data bits %u\n", data_bits);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(parity, "none") == 0) {
		uart_parity = UART_PARITY_NONE;
	} else if (cmd_strcmp(parity, "odd") == 0) {
		uart_parity = UART_PARITY_ODD;
	} else if (cmd_strcmp(parity, "even") == 0) {
		uart_parity = UART_PARITY_EVEN;
	} else {
		CMD_ERR("invalid parity %s\n", parity);
		return CMD_STATUS_INVALID_ARG;
	}

	if (stop_bits < 1 || stop_bits > 2) {
		CMD_ERR("invalid stop bits %u\n", stop_bits);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(flow_ctrl, "none") != 0) {
		CMD_ERR("invalid flow contorl %s\n", flow_ctrl);
		return CMD_STATUS_INVALID_ARG;
	}

	HAL_UART_DeInit((UART_ID)id);
	uart_param.baudRate = baud_rate;
	uart_param.parity = uart_parity;
	uart_param.stopBits = uart_stop_bits[stop_bits - 1];
	uart_param.dataBits = uart_data_bits[data_bits - 5];
	uart_param.isAutoHwFlowCtrl = 0;
	HAL_UART_Init((UART_ID)id, &uart_param);

	return CMD_STATUS_OK;
}

static void cmd_uart_transfer_task(void *arg)
{
	struct cmd_uart_priv *priv = arg;
	struct cmd_uart_msg msg;
	uart_receive_func rx_func;
	uart_transmit_func tx_func;
	uint8_t *buf = NULL;
	int32_t cnt;

	if (priv->mode == CMD_UART_TRANSFER_MODE_IT) {
		rx_func = HAL_UART_Receive_IT;
		tx_func = HAL_UART_Transmit_IT;
	} else if (priv->mode == CMD_UART_TRANSFER_MODE_DMA) {
		rx_func = HAL_UART_Receive_DMA;
		tx_func = HAL_UART_Transmit_DMA;
	} else if (priv->mode == CMD_UART_TRANSFER_MODE_POLL) {
		rx_func = HAL_UART_Receive_Poll;
		tx_func = HAL_UART_Transmit_Poll;
	} else {
		CMD_ERR("invalid mode %d\n", priv->mode);
		goto out;
	}

	buf = cmd_malloc(CMD_UART_TRANSFER_BUF_SIZE);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		goto out;
	}

	if (priv->mode == CMD_UART_TRANSFER_MODE_DMA) {
		if (HAL_UART_EnableRxDMA(priv->id) != HAL_OK) {
			CMD_ERR("enable rx dma fail\n");
			goto out;
		}
		if (HAL_UART_EnableTxDMA(priv->id) != HAL_OK) {
			CMD_ERR("enable tx dma fail\n");
			HAL_UART_DisableRxDMA(priv->id);
			goto out;
		}
	}

	while (1) {
		if (OS_QueueReceive(&priv->queue, &msg, OS_WAIT_FOREVER) != OS_OK) {
			CMD_ERR("wait msg fail\n");
			continue;
		}

		if (msg.len <= 0) {
			CMD_DBG("uart transfer test end\n");
			break;
		}

		cnt = rx_func(priv->id, buf, msg.len, msg.timeout);
		if (cnt > 0) {
			tx_func(priv->id, buf, cnt);
		}
	}

	if (priv->mode == CMD_UART_TRANSFER_MODE_DMA) {
		HAL_UART_DisableTxDMA(priv->id);
		HAL_UART_DisableRxDMA(priv->id);
	}

out:
	if (buf)
		cmd_free(buf);

	CMD_DBG("%s() exit\n", __func__);
	OS_ThreadDelete(&priv->thread);
}

/*
 * drv uart transfer-start i=<id> m=<mode>
 */
static enum cmd_status cmd_uart_transfer_start_exec(char *cmd)
{
	int32_t cnt;
	uint32_t id;
	char mode[8];
	struct cmd_uart_priv *priv = &g_cmd_uart_priv;

	if (OS_ThreadIsValid(&priv->thread)) {
		CMD_ERR("uart transfer already start\n");
		return CMD_STATUS_FAIL;
	}

	cnt = cmd_sscanf(cmd, "i=%u m=%7s", &id, mode);
	if (cnt != 2) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= UART_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	} else {
		priv->id = (UART_ID)id;
	}

	if (cmd_strcmp(mode, "it") == 0) {
		priv->mode = CMD_UART_TRANSFER_MODE_IT;
	} else if (cmd_strcmp(mode, "dma") == 0) {
		priv->mode = CMD_UART_TRANSFER_MODE_DMA;
	} else if (cmd_strcmp(mode, "poll") == 0) {
		priv->mode = CMD_UART_TRANSFER_MODE_POLL;
	} else {
		CMD_ERR("invalid mode %s\n", mode);
		return CMD_STATUS_INVALID_ARG;
	}

	if (OS_QueueCreate(&priv->queue,
	                   CMD_UART_QUEUE_LEN,
	                   sizeof(struct cmd_uart_msg)) != OS_OK) {
		CMD_ERR("create queue failed\n");
		return CMD_STATUS_FAIL;
	}

	if (OS_ThreadCreate(&priv->thread,
		                "",
		                cmd_uart_transfer_task,
		                priv,
		                OS_PRIORITY_NORMAL,
		                CMD_UART_TRANSFER_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create transfer task failed\n");
		OS_QueueDelete(&priv->queue);
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

/*
 * drv uart transfer-data i=<id> l=<data-length> t=<rx-timeout>
 */
static enum cmd_status cmd_uart_transfer_data_exec(char *cmd)
{
	int32_t cnt;
	uint32_t id, len, timeout;
	struct cmd_uart_msg msg;
	struct cmd_uart_priv *priv = &g_cmd_uart_priv;

	if (!OS_ThreadIsValid(&priv->thread)) {
		CMD_ERR("uart transfer not started\n");
		return CMD_STATUS_FAIL;
	}

	cnt = cmd_sscanf(cmd, "i=%u l=%u t=%u", &id, &len, &timeout);
	if (cnt != 3) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id != priv->id) {
		CMD_ERR("invalid id %u, != %d\n", id, priv->id);
		return CMD_STATUS_INVALID_ARG;
	}

	if (len == 0 || len > CMD_UART_TRANSFER_BUF_SIZE) {
		CMD_ERR("invalid data-length %u\n", len);
		return CMD_STATUS_INVALID_ARG;
	}

	if (timeout == 0) {
		timeout = HAL_WAIT_FOREVER;
	}

	msg.len = len;
	msg.timeout = timeout;
	if (OS_QueueSend(&priv->queue, &msg, 0) != OS_OK)
		return CMD_STATUS_FAIL;
	else
		return CMD_STATUS_OK;
}

/*
 * drv uart transfer-stop i=<id>
 */
static enum cmd_status cmd_uart_transfer_stop_exec(char *cmd)
{
	int32_t cnt;
	uint32_t id;
	struct cmd_uart_msg msg;
	struct cmd_uart_priv *priv = &g_cmd_uart_priv;

	if (!OS_ThreadIsValid(&priv->thread)) {
		CMD_ERR("uart transfer not started\n");
		return CMD_STATUS_FAIL;
	}

	cnt = cmd_sscanf(cmd, "i=%u", &id);
	if (cnt != 1) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id != priv->id) {
		CMD_ERR("invalid id %u, != %d\n", id, priv->id);
		return CMD_STATUS_INVALID_ARG;
	}

	msg.len = 0; /* trigger transfer test end */
	msg.timeout = 0;
	if (OS_QueueSend(&priv->queue, &msg, OS_WAIT_FOREVER) != OS_OK)
		return CMD_STATUS_FAIL;

	while (OS_ThreadIsValid(&priv->thread)) {
		OS_MSleep(1); /* wait for thread termination */
	}

	OS_QueueDelete(&priv->queue);
	return CMD_STATUS_OK;
}

/*
 * drv uart sendbreak i=<id> t=<break-length>
 */
static enum cmd_status cmd_uart_sendbreak_exec(char *cmd)
{
	uint32_t id, break_len;
	int32_t cnt;

	cnt = cmd_sscanf(cmd, "i=%u t=%u", &id, &break_len);
	if (cnt != 2) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= UART_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if (break_len < 1 || break_len > 5000) {
		CMD_ERR("invalid break length %u\n", break_len);
		return CMD_STATUS_INVALID_ARG;
	}

	HAL_UART_SetBreakCmd((UART_ID)id, 1);
	OS_MSleep(break_len);
	HAL_UART_SetBreakCmd((UART_ID)id, 0);

	return CMD_STATUS_OK;
}

static struct cmd_data g_uart_cmds[] = {
	{ "config",			cmd_uart_config_exec },
	{ "transfer-start",	cmd_uart_transfer_start_exec },
	{ "transfer-data",	cmd_uart_transfer_data_exec },
	{ "transfer-stop",	cmd_uart_transfer_stop_exec },
	{ "sendbreak",		cmd_uart_sendbreak_exec },
};

enum cmd_status cmd_uart_exec(char *cmd)
{
	return cmd_exec(cmd, g_uart_cmds, cmd_nitems(g_uart_cmds));
}
