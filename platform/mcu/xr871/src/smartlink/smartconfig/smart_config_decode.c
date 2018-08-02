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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

#include "net/wlan/wlan_defs.h"

#include "smartlink/smart_config/wlan_smart_config.h"
#include "smart_config.h"
#include "smart_config_crc.h"
#include "smart_config_decode.h"

#define g_debuglevel  ERROR

enum LEADCODE_SEQ {
	LEAD_CODE_NOME = 0,             /* 0, not used */
	LEAD_CODE_GET_CHANNEL,          /* 1 */
	LEAD_CODE_GET_SSIDPWD_SIZE,     /* 2 */
	LEAD_CODE_GET_PWD_SIZE,         /* 3 */
	LEAD_CODE_GET_ROUND_NUM,        /* 4 */
	LEAD_CODE_COMPLETE,             /* 5, not used */
};

#define LOCDED_FIELD 4

static int dec_valid_packet_count(uint8_t *count_pkt, uint8_t packet_num)
{
	/* count Success return 1, else return 0 */
	int check;

	if (packet_num > 127)
		return 0;

	/* if data already exist, return 1, else return 0; if data is null return -1 */
	check = (((*(count_pkt + packet_num / 8)) & (1 << (8 - (packet_num % 8) - 1))) > 0);
	if (check == 1)
		return 0;

	*(count_pkt + packet_num / 8) |= 1 << (8 - (packet_num % 8) - 1);

	return 1;
}

static enum LEADCODE_SEQ
dec_get_lead_code(struct ieee80211_frame *iframe, sc_lead_code_t *lead_code,
                  uint8_t packet_num, SMART_CONFIG_STATUS_T *status)
{
	uint8_t *sa = iframe->i_addr2;
	uint8_t *data = &iframe->i_addr3[4];
	uint8_t *crc = &iframe->i_addr3[5];
	uint8_t crc8;

	if (packet_num == LEAD_CODE_NOME || packet_num > LEAD_CODE_GET_ROUND_NUM) {
		SC_DBG(ERROR, "%s,%d packet_num:%d", __func__, __LINE__, packet_num);
		return LEAD_CODE_NOME;
	}

	/* the leadcode complete return 1,else return 0 */
	if ((*status != SC_STATUS_LOCKED_CHAN) ||
	    (lead_code->ssidpwd_len == 0) || (lead_code->pwd_len == -1) ||
	    (lead_code->random == 0)) {
		;//SC_DBG(INFO, "%s, %d\n", __func__, __LINE__);
	} else {
		SC_DBG(INFO, "lead complete ssid+pwd len:%d pwd len:%d "
		       "random:%d channel:%d\n", lead_code->ssidpwd_len,
		       lead_code->pwd_len, lead_code->random, lead_code->channel);
		if (lead_code->locked_mac_flag) {
			lead_code->locked_mac_flag = 0;
		}
		return LEAD_CODE_COMPLETE;
	}

	 crc8 = cal_crc8(data, 1); /* crc check the leadcode */
	if (crc8 != *crc)
		return LEAD_CODE_NOME;

	SC_DBG(INFO, "get leadcode %d\n", packet_num);
	if (!lead_code->locked_mac_flag) {
		SC_DBG(INFO, "locked mac:%x %x\n", sa[4], sa[5]);
		//memcpy(lead_code->locked_mac, sa, 6);
		lead_code->locked_mac_flag = 1;
	}

	switch (packet_num) {
	case LEAD_CODE_GET_CHANNEL:
		lead_code->channel = *data;
		*status = SC_STATUS_LOCKED_CHAN;
		SC_DBG(INFO, "CHANNEL LOCKED : %d\n", lead_code->channel);
		return LEAD_CODE_GET_CHANNEL;
	case LEAD_CODE_GET_SSIDPWD_SIZE:
		lead_code->ssidpwd_len = *data;
		lead_code->packet_total = lead_code->ssidpwd_len / 2 +
		                          1 + lead_code->ssidpwd_len % 2;
		SC_DBG(INFO, "PACKET_SUM %d \n", lead_code->packet_total);
		return LEAD_CODE_GET_SSIDPWD_SIZE;
	case LEAD_CODE_GET_PWD_SIZE:
		lead_code->pwd_len = *data;
		return LEAD_CODE_GET_PWD_SIZE;
	case LEAD_CODE_GET_ROUND_NUM:
		lead_code->random = *data;
		return LEAD_CODE_GET_ROUND_NUM;
	}

	return LEAD_CODE_NOME;
}

static int dec_data_decode(smartconfig_priv_t *priv, sc_result_t *result,
                           sc_lead_code_t *lead_code, uint8_t *src_data_buff)
{
	int i = 0;
	uint8_t src_ssid_size = 0;
	uint8_t src_pwd_size = 0;
	uint8_t crc8_pwd = 0;
	uint8_t crc8_ssid = 0;
	uint8_t *src_pwd_data = NULL;
	uint8_t *src_ssid_data = NULL;

	src_ssid_size = lead_code->ssidpwd_len - lead_code->pwd_len;
	src_pwd_size = lead_code->pwd_len;
	SC_DBG(INFO, "DATA_DECODE PWD_SIZE : %d SSID_SIZE: %d\n",
	       src_pwd_size, src_ssid_size);

	src_pwd_data = src_data_buff;
	src_ssid_data = src_data_buff + src_pwd_size;

	if (priv->aes_key[0] != 0) {
		uint8_t temp_pwd[66];
		uint8_t temp_ssid[65];

		if (src_pwd_size >= 66 || src_ssid_size >= 65) {
			SC_DBG(ERROR, "DATA_DECODE pwd:\n");
			return -1;
		}
		memset(temp_pwd , 0, 66);
		memset(temp_ssid, 0, 65);

		SC_DBG(INFO, "DATA_DECODE pwd:\n");

		if (src_pwd_size) {
			if (aes_ebc_decrypt((char *)src_pwd_data, (char *)temp_pwd,
		                            src_pwd_size, priv->aes_key) == 0) {
				int pwd_dlen;

				pwd_dlen = *(temp_pwd + src_pwd_size - 1);
				SC_DBG(INFO, "pwd_dlen: %d\n", pwd_dlen);
				for (i = 0; i < pwd_dlen; i++) {
					int value = *(temp_pwd + src_pwd_size - 1 - i);
					if (value != pwd_dlen) {
						SC_DBG(ERROR, "%s,%d, aes pwd err, "
						       "value:%d\n", __func__, __LINE__, value);
						return -1;
					}
				}
				result->pwd_len = src_pwd_size - pwd_dlen;
				memcpy(result->pwd, temp_pwd, strlen((const char *)temp_pwd));
			} else
				SC_DBG(ERROR, "%s,%d, aes pwd err\n", __func__, __LINE__);
		}
		if (src_ssid_size) {
			if (aes_ebc_decrypt((char *)src_ssid_data, (char *)temp_ssid,
			                    src_ssid_size, priv->aes_key) == 0) {
				int ssid_dlen = *(temp_ssid + src_ssid_size - 1);

				SC_DBG(INFO, "ssid_dlen: %d\n", ssid_dlen);
				for (i = 0; i < ssid_dlen; i++) {
					int value = *(temp_ssid + src_ssid_size - 1 - i);
					if (value != ssid_dlen) {
						SC_DBG(ERROR, "%s,%d, aes ssid err, value:%d\n",
						       __func__, __LINE__, value);
						return -1;
					}
				}
				result->ssid_len = src_ssid_size- ssid_dlen;
				memcpy(result->ssid, temp_ssid, strlen((const char *)temp_ssid));
			} else {
				SC_DBG(ERROR, "%s,%d, aes ssid err\n",  __func__, __LINE__);
			}
		}

		*(result->pwd + result->pwd_len) = 0;
		*(result->ssid + result->ssid_len) = 0;
	} else if (priv->aes_key[0] == 0) {
		result->ssid_len = src_ssid_size;
		result->pwd_len = src_pwd_size;

		memcpy(result->ssid, src_ssid_data, result->ssid_len);
		memcpy(result->pwd, src_pwd_data, result->pwd_len);

		*(result->pwd + result->pwd_len) = 0;
		*(result->ssid + result->ssid_len) = 0;
	}

	result->random = lead_code->random;
	SC_DBG(INFO, "ssid:%s pwd:%s\n", result->ssid, result->pwd);

	i = (lead_code->ssidpwd_len % 2);

	crc8_pwd = *(src_data_buff + lead_code->ssidpwd_len + i);

	if (cal_crc8(result->pwd, result->pwd_len) != crc8_pwd) {
		src_data_buff[0] = 0;
		SC_DBG(ERROR, "%s,%d pwd crc err %x\n", __func__, __LINE__, crc8_pwd);

		return -1;
	}

	crc8_ssid = *(src_data_buff + lead_code->ssidpwd_len + i + 1);

	if (cal_crc8(result->ssid, result->ssid_len) != crc8_ssid) {
		src_data_buff[0] = 0;
		SC_DBG(ERROR, "%s,%d ssid crc err\n", __func__, __LINE__);

		return -1;
	}

	src_data_buff[0] = 0;

	return 0;
}

/* save psk and ssid data */
static int
dec_push_data(sc_lead_code_t *lead_code, uint8_t packet_num,
              uint16_t *src_data_buff, uint16_t data)
{
	int d = 0;

	packet_num -= 5;

	if (dec_valid_packet_count(lead_code->count_pkt, packet_num) == 1)
		lead_code->packet_count += 1;

	SC_DBG(INFO, "push packet_num:%d, data:%x\n", packet_num, data);

	*(src_data_buff + packet_num) = data;
	d = lead_code->packet_count - lead_code->packet_total;

	return d;
}

void sc_reset_lead_code(sc_lead_code_t *lead_code)
{
	memset(lead_code , 0, sizeof(sc_lead_code_t));
	lead_code->pwd_len = -1;
}

SMART_CONFIG_STATUS_T
sc_dec_packet_decode(smartconfig_priv_t *priv, uint8_t *data, uint32_t len)
{
	struct ieee80211_frame *iframe = (struct ieee80211_frame *)data;
	uint8_t packet_num = iframe->i_addr3[3];
	int ret;
	SMART_CONFIG_STATUS_T status = priv->status;
	sc_lead_code_t *lead_code = &priv->lead_code;

	if (!iframe) {
		SC_DBG(ERROR, "%s,%d\n", __func__, __LINE__);
		return status;
	}

	if ((iframe->i_addr3[0] == SC_MAGIC_ADD0) &&
	    (iframe->i_addr3[1] == SC_MAGIC_ADD1) &&
	    (iframe->i_addr3[2] == SC_MAGIC_ADD2))
	      ;
	else
		return status;

	if (packet_num > LEAD_CODE_NOME && packet_num < LEAD_CODE_COMPLETE) {
		if (!lead_code->lead_complete_flag) {
			status = priv->status;
			ret = dec_get_lead_code(iframe, lead_code, packet_num, &status);
			if (ret == LEAD_CODE_COMPLETE)
				lead_code->lead_complete_flag = 1;
		}

		return status;
	}

	if (!lead_code->lead_complete_flag)
		return status;

	if (packet_num <= 127) {
		uint16_t data = (iframe->i_addr3[4]) | (iframe->i_addr3[5] << 8);

		if (dec_push_data(lead_code, packet_num,
		    (uint16_t *)priv->src_data_buff, data) != 0)
			return status;
	}

	if (priv->status < SC_STATUS_COMPLETE &&
	    lead_code->packet_total == lead_code->packet_count){
		SC_DBG(INFO, "DATA IS ALL RECEIVE, data_decode\n");
		ret = dec_data_decode(priv, &priv->result, lead_code, priv->src_data_buff);
		if (ret) {
			SC_DBG(INFO, "decode err\n");
			sc_reset_lead_code(&priv->lead_code);
			priv->src_data_buff[0] = 0;
			status = SC_STATUS_END;
		} else {
			SC_DBG(INFO, "decode success\n");
			status = SC_STATUS_COMPLETE;
		}
	}

	return status;
}

uint16_t sc_read_locked_channel(smartconfig_priv_t *priv)
{
	return priv->lead_code.channel;
}
