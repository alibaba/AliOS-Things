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
#include "driver/chip/hal_uart.h"
#include "driver/chip/hal_crypto.h"

static uint8_t efpg_checksum8(uint8_t *data, uint32_t len)
{
	uint8_t cs = 0;

	while (len > 0) {
		cs += *data++;
		len--;
	}

	return cs;
}

static int efpg_check_msg_dgst(efpg_priv_t *efpg)
{
	CE_SHA256_Handler hdl;
	uint8_t msg_dgst_cal[EFPG_MSG_DGST_LEN];
	uint8_t *msg_dgst = efpg->data_frame + efpg->recv_len - EFPG_MSG_DGST_LEN;

	if ((HAL_SHA256_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, efpg->cmd_frame, EFPG_CMD_FRAME_LEN) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, efpg->data_frame, efpg->recv_len - EFPG_MSG_DGST_LEN) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, efpg->key, efpg->key_len) != HAL_OK)
		|| (HAL_SHA256_Finish(&hdl, (uint32_t *)msg_dgst_cal) != HAL_OK)) {
		EFPG_ERR("failed to calculate msg dgst\n");
		return -1;
	}

	if (efpg_memcmp(msg_dgst, msg_dgst_cal, EFPG_MSG_DGST_LEN)) {
		EFPG_WARN("%s(), %d, check msg dgst failed\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

static int efpg_send_ack(efpg_priv_t *efpg, uint16_t status)
{
	uint8_t cs;
	int32_t send_len;

	uint8_t *ack_frame = efpg_malloc(EFPG_ACK_FRAME_LEN);
	if (ack_frame == NULL) {
		EFPG_ERR("malloc failed\n");
		return -1;
	}

	efpg_memset(ack_frame, 0, EFPG_ACK_FRAME_LEN);
	efpg_memcpy(ack_frame, &status, 2);
	cs = 0xFF - efpg_checksum8(ack_frame, EFPG_ACK_FRAME_LEN);
	efpg_memcpy(ack_frame + 3, &cs, sizeof(cs));

	send_len = HAL_UART_Transmit_Poll(efpg->uart_id, ack_frame, EFPG_ACK_FRAME_LEN);
	efpg_free(ack_frame);

	if (send_len != EFPG_ACK_FRAME_LEN) {
		EFPG_WARN("%s(), %d, send len %d\n", __func__, __LINE__, send_len);
		return -1;
	} else {
		return 0;
	}
}

static int efpg_parse_cmd(efpg_priv_t *efpg)
{
	uint16_t	op_code;
	uint16_t	type;
	uint16_t	len;
	uint8_t	   *p;
	uint8_t	   extCmd;

	if (efpg->ext_cmd == EFPG_EXT_CMD)
		p = efpg->ext_cmd_frame;
	else
		p = efpg->cmd_frame;

	op_code = *((uint16_t *)p);
	p += 2;
	type = *((uint16_t *)p);
	p += 2;
	len = *((uint16_t *)p);

	switch (op_code) {
	case EFPG_OP_CODE_READ:
		efpg->op = EFPG_OP_READ;
		break;
	case EFPG_OP_CODE_WRITE:
		efpg->op = EFPG_OP_WRITE;
		break;
	case EFPG_OP_CODE_EXIT:
		efpg->op = EFPG_OP_EXIT;
		return 0;
	default:
		EFPG_WARN("%s(), %d, op_code %#06x\n", __func__, __LINE__, op_code);
		return -1;
	}

	switch (type) {
	case EFPG_TYPE_HOSC:
		efpg->field = EFPG_FIELD_HOSC;
		efpg->expt_len = EFPG_HOSC_FRAME_LEN;
		break;
	case EFPG_TYPE_BOOT:
		efpg->field = EFPG_FIELD_BOOT;
		efpg->expt_len = EFPG_BOOT_FRAME_LEN;
		break;
	case EFPG_TYPE_DCXO:
		efpg->field = EFPG_FIELD_DCXO;
		efpg->expt_len = EFPG_DCXO_FRAME_LEN;
		break;
	case EFPG_TYPE_POUT:
		efpg->field = EFPG_FIELD_POUT;
		efpg->expt_len = EFPG_POUT_FRAME_LEN;
		break;
	case EFPG_TYPE_MAC:
		efpg->field = EFPG_FIELD_MAC;
		efpg->expt_len = EFPG_MAC_FRAME_LEN;
		break;
	case EFPG_TYPE_USER_AREA:
		if (efpg->ext_cmd == EFPG_NORMAL_CMD) {
			p += 2;
			extCmd = *p;
			if (extCmd != EFPG_EXT_CMD) {
				EFPG_WARN("%s(), %d, extCmd %d, expt extCmd 0\n",
						  __func__, __LINE__, extCmd);
				return -1;
			}
			efpg->ext_cmd = EFPG_EXT_CMD;
			efpg->field = EFPG_FIELD_UA;
			efpg->expt_len = EFPG_UAER_AREA_EXT_LEN;
		}
		else {
			p += 2;
			efpg->protocol_version = *((uint16_t *)p);
			p += 2;
			efpg->start_bit_addr= *((uint16_t *)p);
			p += 2;
			efpg->bit_length = *((uint16_t *)p);
			efpg->ext_cmd = EFPG_NORMAL_CMD;
			efpg->field = EFPG_FIELD_UA;
			efpg->expt_len = efpg->bit_length / 8 + EFPG_MSG_DGST_LEN;
			if (efpg->bit_length % 8)
				efpg->expt_len ++;
			len = efpg->expt_len;
		}
		break;
	default:
		EFPG_WARN("%s(), %d, type %#06x\n", __func__, __LINE__, type);
		return -1;
	}

	if (len != efpg->expt_len) {
		EFPG_WARN("%s(), %d, len %d, expt len %d\n",
				  __func__, __LINE__, len, efpg->expt_len);
		return -1;
	}

	return 0;
}

static efpg_state_t efpg_ext_cmd_process(efpg_priv_t *efpg)
{
	efpg->is_cmd = 1;
	efpg->recv_len = 0;

	if (efpg_send_ack(efpg, EFPG_ACK_OK) < 0) {
		EFPG_WARN("%s(), %d, send ack failed\n", __func__, __LINE__);
		return EFPG_STATE_RESET;
	}

	return EFPG_STATE_CONTINUE;
}

static efpg_state_t efpg_read_process(efpg_priv_t *efpg)
{
	uint16_t status;
	uint8_t *data;
	uint8_t *msg_dgst;
	int32_t send_len;
	CE_SHA256_Handler hdl;

	uint8_t *frame = efpg_malloc(efpg->expt_len);
	if (frame == NULL) {
		EFPG_ERR("malloc failed\n");
		return EFPG_STATE_RESET;
	}

	efpg_memset(frame, 0, efpg->expt_len);
	data = frame;
	msg_dgst = frame + efpg->expt_len - EFPG_MSG_DGST_LEN;

	status = efpg_read_field(efpg->field, data, efpg->start_bit_addr, efpg->bit_length);
	if ((HAL_SHA256_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, efpg->cmd_frame, EFPG_CMD_FRAME_LEN) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, data, efpg->expt_len - EFPG_MSG_DGST_LEN) != HAL_OK)
		|| (HAL_SHA256_Append(&hdl, efpg->key, efpg->key_len) != HAL_OK)
		|| (HAL_SHA256_Finish(&hdl, (uint32_t *)msg_dgst) != HAL_OK)) {
		EFPG_WARN("%s(), %d, SHA256 failed\n", __func__, __LINE__);
		status = EFPG_ACK_RW_ERR;
	}

	if (status != EFPG_ACK_OK) {
		EFPG_WARN("%s(), %d, status %d\n", __func__, __LINE__, status);
		efpg_free(frame);
		efpg_send_ack(efpg, status);
		return EFPG_STATE_RESET;
	}

	if (efpg_send_ack(efpg, EFPG_ACK_OK) < 0) {
		EFPG_WARN("%s(), %d, send ack failed\n", __func__, __LINE__);
		efpg_free(frame);
		return EFPG_STATE_RESET;
	}

	send_len = HAL_UART_Transmit_Poll(efpg->uart_id, frame, efpg->expt_len);
	if (send_len != efpg->expt_len)
		EFPG_WARN("%s(), %d, send len %d, expt %d\n",
				  __func__, __LINE__, send_len, efpg->expt_len);

	efpg_free(frame);
	return EFPG_STATE_RESET;
}

static efpg_state_t efpg_write_process(efpg_priv_t *efpg)
{
	efpg->is_cmd = 0;
	efpg->recv_len = 0;

	if (efpg_send_ack(efpg, EFPG_ACK_OK) < 0) {
		EFPG_WARN("%s(), %d, send ack failed\n", __func__, __LINE__);
		return EFPG_STATE_RESET;
	}

	return EFPG_STATE_CONTINUE;
}

static efpg_state_t efpg_stop_process(efpg_priv_t *efpg)
{
	if (efpg_send_ack(efpg, EFPG_ACK_OK) < 0) {
		EFPG_WARN("%s(), %d, send ack failed\n", __func__, __LINE__);
		return EFPG_STATE_RESET;
	} else {
		return EFPG_STATE_STOP;
	}
}

efpg_state_t efpg_cmd_frame_process(efpg_priv_t *efpg)
{
	uint8_t * pFrame;
	if (efpg->ext_cmd == EFPG_EXT_CMD)
		pFrame = efpg->ext_cmd_frame;
	else
		pFrame = efpg->cmd_frame;

	/* checksum */
	if (efpg_checksum8(pFrame, efpg->recv_len) != 0xFF) {
		EFPG_WARN("%s(), %d, checksum failed\n", __func__, __LINE__);
		efpg_send_ack(efpg, EFPG_ACK_CS_ERR);
		return EFPG_STATE_RESET;
	}

	/* parse frame */
	if (efpg_parse_cmd(efpg) < 0) {
		EFPG_WARN("%s(), %d, parse cmd failed\n", __func__, __LINE__);
		efpg_send_ack(efpg, EFPG_ACK_PARSE_ERR);
		return EFPG_STATE_RESET;
	}

	/* check ext cmd */
	if (efpg->ext_cmd == EFPG_EXT_CMD) {
		return efpg_ext_cmd_process(efpg);
	}

	switch (efpg->op) {
	case EFPG_OP_READ:
		return efpg_read_process(efpg);
	case EFPG_OP_WRITE:
		return efpg_write_process(efpg);
	case EFPG_OP_EXIT:
		return efpg_stop_process(efpg);
	default:
		EFPG_ERR("invalid op %d\n", efpg->op);
		return EFPG_STATE_STOP;
	}
}

efpg_state_t efpg_data_frame_process(efpg_priv_t *efpg)
{
	uint16_t status;
	uint8_t *data;

	/* message digest */
	if (efpg_check_msg_dgst(efpg) < 0) {
		EFPG_WARN("%s(), %d, check msg dgst failed\n", __func__, __LINE__);
		efpg_send_ack(efpg, EFPG_ACK_MD_ERR);
		return EFPG_STATE_RESET;
	}

	/* write data */
	data = efpg->data_frame;
	status = efpg_write_field(efpg->field, data, efpg->start_bit_addr, efpg->bit_length);
	EFPG_DBG("%s(), %d, write field status %d\n", __func__, __LINE__, status);

	efpg_send_ack(efpg, status);
	return EFPG_STATE_RESET;
}

