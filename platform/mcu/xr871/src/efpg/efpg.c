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

#include "efpg_i.h"
#include "efpg_debug.h"
#include "efpg/efpg.h"
#include "driver/chip/hal_uart.h"
#include "driver/chip/hal_efuse.h"

static void efpg_task(void *arg)
{
	uint8_t *buf;
	int32_t recv_len;
	efpg_priv_t *efpg = arg;
	efpg_state_t state = EFPG_STATE_NUM;

	if (efpg->start_cb)
		efpg->start_cb();

efpg_reset:
	efpg->is_cmd	= 1;
	efpg->ext_cmd	= EFPG_NORMAL_CMD;
	efpg->expt_len	= EFPG_CMD_FRAME_LEN;
	efpg->recv_len	= 0;
	efpg->op		= EFPG_OP_NUM;
	efpg->field		= EFPG_FIELD_NUM;
	efpg->start_bit_addr = 0;
	efpg->bit_length = 0;

	while (1) {
efpg_continue:
		if (efpg->is_cmd)
			if (efpg->ext_cmd == EFPG_EXT_CMD)
				buf = efpg->ext_cmd_frame;
			else
				buf = efpg->cmd_frame;
		else
			buf = efpg->data_frame;
		recv_len = 0;
		while (recv_len == 0) {
			recv_len = HAL_UART_Receive_IT(efpg->uart_id, buf, efpg->expt_len, EFPG_RECV_TIMEOUT_MS);
		}

		if (recv_len == -1) {
			EFPG_ERR("UART receive failed\n");
			goto efpg_stop;
		}

		if ((uint16_t)recv_len != efpg->expt_len) {
			EFPG_WARN("%s(), %d, recv len %d, expt len %d\n",
					  __func__, __LINE__, recv_len, efpg->expt_len);
			goto efpg_reset;
		}

		efpg->recv_len = (uint16_t)recv_len;

		if (efpg->is_cmd)
			state = efpg_cmd_frame_process(efpg);
		else
			state = efpg_data_frame_process(efpg);

		switch (state) {
		case EFPG_STATE_CONTINUE:
			goto efpg_continue;
		case EFPG_STATE_RESET:
			goto efpg_reset;
		case EFPG_STATE_STOP:
			goto efpg_stop;
		default:
			EFPG_ERR("invalid state %d\n", state);
			goto efpg_stop;
		}
	}

efpg_stop:
	if (efpg->stop_cb)
		efpg->stop_cb();

	if (efpg)
		efpg_free(efpg);

	OS_ThreadDelete(NULL);
}

/**
 * @brief Start communicating with the OEM programming tool
 * @param[in] key Pointer to the key
 * @param[in] key_len The length of the key
 * @param[in] uart_id ID of the specified UART used to communication
 * @param[in] start_cb Function called when start communication
 * @param[in] stop_cb Function called when stop communication
 * @return 0 on success, -1 on failure
 *
 * @note Just for OEM programming tool to entry eFuse programming mode.
 */
int efpg_start(uint8_t *key, uint8_t key_len, UART_ID uart_id, efpg_cb_t start_cb, efpg_cb_t stop_cb)
{
	efpg_priv_t *efpg;
	OS_Thread_t thread;

	if ((key == NULL) || (key_len == 0) || (key_len > EFPG_KEY_LEN_MAX) || (uart_id == UART_NUM)) {
		EFPG_ERR("key %p, key len %d, uart id %d\n", key, key_len, uart_id);
		return -1;
	}

	efpg = efpg_malloc(sizeof(efpg_priv_t));
	if (efpg == NULL) {
		EFPG_ERR("malloc failed\n");
		return -1;
	}

	efpg->uart_id = uart_id;
	efpg->start_cb = start_cb;
	efpg->stop_cb = stop_cb;

	efpg_memcpy(efpg->key, key, key_len);
	efpg->key_len = key_len;

	OS_ThreadSetInvalid(&thread);
	if (OS_ThreadCreate(&thread,
						"efpg task",
						efpg_task,
						efpg,
						OS_THREAD_PRIO_CONSOLE,
						EFPG_THREAD_STACK_SIZE) != OS_OK) {
		EFPG_ERR("create efpg task failed\n");
		return -1;
	}

	return 0;
}

/**
 * @brief Read data of the specified field in eFuse
 * @param[in] field The filed in eFuse
 * @param[in] data Pointer to the data buffer
 * @return 0 on success, -1 on failure
 *
 * @note The rest bit(s) in data will be cleared to be 0.
 */
int efpg_read(efpg_field_t field, uint8_t *data)
{
	if (data == NULL) {
		EFPG_ERR("data %p\n", data);
		return -1;
	}

	uint16_t ack = efpg_read_field(field, data, 0, 0);
	if (ack != EFPG_ACK_OK) {
		EFPG_WARN("%s(), %d, ack %d\n", __func__, __LINE__, ack);
		return -1;
	}

	return 0;
}

/**
 * @brief Read data from user area (OEM reserved field) on EFUSE
 * @param[in] start The first bit to be read in user area (OEM reserved field)
 * @param[in] num Number of bits to be read
 * @param[in] data Pointer to the data buffer
 * @return 0 on success, -1 on failure
 *
 * @note The rest bit(s) in data will be cleared to be 0.
 */
int efpg_read_ua(uint32_t start, uint32_t num, uint8_t *data)
{
	if (efpg_read_field(EFPG_FIELD_UA, data, start, num) != EFPG_ACK_OK) {
		return -1;
	}
	return 0;
}

