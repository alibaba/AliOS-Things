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

#include "smart_config_crc.h"
#include "smart_config_decode.h"

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define g_debuglevel  ERROR

#define SC_DECODE_DBG(level, fmt, args...)                      \
	do {                                                    \
		if (level <= g_debuglevel)                      \
			printf("[smart config]"fmt,##args);     \
	} while (0)

#define __SC_BUG() \
		do { printf("BUG at %s:%d!\n", __func__, __LINE__); while (1);} while (0)

#define SC_BUG_ON(condition) do { if (condition) __SC_BUG(); } while(0)

#define LOCDED_FIELD 4

enum LEADCODE_SEQ {
	LEAD_CODE_NOME,
	LEAD_CODE_GET_CHANNEL,
	LEAD_CODE_GET_SSIDPWD_SIZE,
	LEAD_CODE_GET_PWD_SIZE,
	LEAD_CODE_GET_ROUND_NUM,
	LEAD_CODE_COMPLETE,
};

#define IEEE80211_ADDR_LEN 6

struct ieee80211_frame {
	uint8_t		i_fc[2];
	uint8_t		i_dur[2];
	uint8_t		i_addr1[IEEE80211_ADDR_LEN];
	uint8_t		i_addr2[IEEE80211_ADDR_LEN];
	uint8_t		i_addr3[IEEE80211_ADDR_LEN];
	uint8_t		i_seq[2];
	/* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
	/* see below */
} __packed;

extern void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
			  const void *buf, size_t len);

static int dec_valid_packet_count(uint8_t *count_pkt, uint8_t packet_num)
{
	/* count Success return 1, else return 0 */
	int check;

	if (packet_num > 127)
		return 0;

	/* if data already exist, return 1,else return 0; if data is null return -1 */
	check = (((*(count_pkt + packet_num / 8)) & (1 << (8 - (packet_num % 8) - 1))) > 0);
	if (check == 1)
		return 0;

	*(count_pkt + packet_num / 8) |= 1 << (8 - (packet_num % 8) - 1);

	return 1;
}

static enum LEADCODE_SEQ dec_get_lead_code(SC_Priv *sc_priv, struct ieee80211_frame *iframe, SC_Lead_Code *lead_code)
{
	uint8_t *sa = iframe->i_addr2;
	uint8_t packet_num = iframe->i_addr3[3];
	uint8_t *data = &iframe->i_addr3[4];
	uint8_t *crc = &iframe->i_addr3[5];

	/* the leadcode complete return 1,else return 0 */
	if ((sc_priv->Sc_Status != SC_LOCKED_CHAN) || \
	    (lead_code->ssidpwd_size == 0) || (lead_code->pwd_size == -1) || \
	    (lead_code->random_num == 0)) {
		;//SC_DECODE_DBG(INFO, "%s, %d\n", __func__, __LINE__);
	} else {
		SC_DECODE_DBG(INFO, "lead complete!!!\n");
		SC_DECODE_DBG(INFO, "ssidpwd_size : %d\n", lead_code->ssidpwd_size);
		SC_DECODE_DBG(INFO, "pwd_size : %d\n", lead_code->pwd_size);
		SC_DECODE_DBG(INFO, "round_num : %d\n", lead_code->random_num);
		SC_DECODE_DBG(INFO, "channel : %d\n", lead_code->channel);
		if (sc_priv->locked_mac_flag) {
			sc_priv->locked_mac_flag = 0;
		}
		return LEAD_CODE_COMPLETE;
	}

	if (packet_num > 0 && packet_num <= LOCDED_FIELD) {
		/*crc check the leadcode*/
		uint8_t crc8 = cal_crc8(data, 1);
		if (crc8 != *crc)
			return LEAD_CODE_NOME;

		SC_DECODE_DBG(INFO, "get_leadcode\n");
		if (!sc_priv->locked_mac_flag) {
			SC_DECODE_DBG(INFO, "locked src:\n");
			memcpy(sc_priv->locked_mac, sa, 6);
			sc_priv->locked_mac_flag = 1;
			print_hex_dump_bytes(NULL, 0, sc_priv->locked_mac, 6);
		}

		if(packet_num == LEAD_CODE_GET_CHANNEL) {
			lead_code->channel = *data;
			sc_priv->Sc_Status = SC_LOCKED_CHAN;
			SC_DECODE_DBG(INFO, "CHANNEL LOCKED : %d\n", lead_code->channel);
			return  LEAD_CODE_GET_CHANNEL;
		} else if (packet_num == LEAD_CODE_GET_SSIDPWD_SIZE) {
			lead_code->ssidpwd_size = *data;
			lead_code->packet_sum = lead_code->ssidpwd_size / 2 + 1 + lead_code->ssidpwd_size % 2;
			SC_DECODE_DBG(INFO, "PACKET_SUM %d \n", lead_code->packet_sum);
			return LEAD_CODE_GET_SSIDPWD_SIZE;
		} else if (packet_num == LEAD_CODE_GET_PWD_SIZE) {
			lead_code->pwd_size = *data;
			return LEAD_CODE_GET_PWD_SIZE;
		} else if (packet_num == LEAD_CODE_GET_ROUND_NUM) {
			lead_code->random_num = *data;
			return LEAD_CODE_GET_ROUND_NUM;
		}
	}

	return LEAD_CODE_NOME;
}

static int dec_data_decode(SC_Priv *sc_priv, SC_Result *result, SC_Lead_Code *lead_code, uint8_t *src_data_buff)
{
	int i = 0;
	uint8_t src_ssid_size = 0;
	uint8_t src_pwd_size = 0;
	uint8_t crc8_pwd = 0;
	uint8_t crc8_ssid = 0;
	uint8_t *src_pwd_data = NULL;
	uint8_t *src_ssid_data = NULL;

	src_ssid_size = lead_code->ssidpwd_size - lead_code->pwd_size;
	src_pwd_size = lead_code->pwd_size;
	SC_DECODE_DBG(INFO, "DATA_DECODE PWD_SIZE : %d 	 SSID_SIZE : %d\n",\
	              src_pwd_size, src_ssid_size);

	src_pwd_data = src_data_buff;
	src_ssid_data = src_data_buff + src_pwd_size;

	if (sc_priv->Aes_Key[0] != 0) {
		uint8_t temp_pwd[66];
		uint8_t temp_ssid[65];

		SC_BUG_ON(src_pwd_size >= 66);
		SC_BUG_ON(src_ssid_size >= 65);
		memset(temp_pwd , 0, 66);
		memset(temp_ssid, 0, 65);

		SC_DECODE_DBG(INFO, "DATA_DECODE pwd:\n");

		if (src_pwd_size) {
			if (aes_ebc_decrypt((char *)src_pwd_data, (char *) \
		                     temp_pwd, src_pwd_size, sc_priv->Aes_Key) == 0) {
				int pwd_dlen;

				pwd_dlen = *(temp_pwd + src_pwd_size - 1);
				SC_DECODE_DBG(INFO, "pwd_dlen: %d\n", pwd_dlen);
				for (i = 0; i < pwd_dlen; i++) {
					int value = *(temp_pwd + src_pwd_size - 1 - i);
					if (value != pwd_dlen) {
						SC_DECODE_DBG(ERROR, "%s(), %d, aes pwd err, value:%d\n", __func__, __LINE__, value);
						return 0;
					}
				}
				result->pwd_size = src_pwd_size - pwd_dlen;
				memcpy(result->pwd, temp_pwd, strlen((const char *)temp_pwd));
			} else
				SC_DECODE_DBG(ERROR, "%s,%d, aes pwd err\n", __func__, __LINE__);
		}
		if (src_ssid_size) {
			if (aes_ebc_decrypt((char *)src_ssid_data, \
		                     	(char *)temp_ssid, src_ssid_size, sc_priv->Aes_Key) == 0) {
				int ssid_dlen = *(temp_ssid + src_ssid_size - 1);

				SC_DECODE_DBG(INFO, "ssid_dlen: %d\n", ssid_dlen);
				for (i = 0; i < ssid_dlen; i++) {
					int value = *(temp_ssid + src_ssid_size - 1 - i);
					if (value != ssid_dlen) {
						SC_DECODE_DBG(ERROR, "%s,%d, aes ssid err, value:%d\n",
					            	  __func__, __LINE__, value);
						return 0;
					}
				}
				result->ssid_size = src_ssid_size- ssid_dlen;
				memcpy(result->ssid, temp_ssid, strlen((const char *)temp_ssid));
			} else
				SC_DECODE_DBG(ERROR, "%s,%d, aes ssid err\n",  __func__, __LINE__);
		}

		*(result->pwd + result->pwd_size) = 0;
		*(result->ssid + result->ssid_size) = 0;
	} else if (sc_priv->Aes_Key[0] == 0) {
		SC_BUG_ON(src_ssid_size >= 65);
		SC_BUG_ON(src_pwd_size >= 66);
		result->ssid_size = src_ssid_size;
		result->pwd_size = src_pwd_size;

		memcpy(result->ssid, src_ssid_data, result->ssid_size);
		memcpy(result->pwd, src_pwd_data, result->pwd_size);

		*(result->pwd + result->pwd_size) = 0;
		*(result->ssid + result->ssid_size) = 0;
	}

	result->random_num = lead_code->random_num;
	SC_DECODE_DBG(INFO, "pwd:%s, ssid:%s\n", result->pwd, result->ssid);

	i = (lead_code->ssidpwd_size % 2);

	crc8_pwd = *(src_data_buff + lead_code->ssidpwd_size + i);

	if (cal_crc8(result->pwd, result->pwd_size) != crc8_pwd) {
		src_data_buff[0] = 0;
		SC_DECODE_DBG(ERROR, "%s,%d pwd crc check err\n", __func__, __LINE__);

		return 0;
	}

	crc8_ssid = *(src_data_buff + lead_code->ssidpwd_size + i + 1);

	if (cal_crc8(result->ssid, result->ssid_size) != crc8_ssid) {
		src_data_buff[0] = 0;
		SC_DECODE_DBG(ERROR, "%s,%d ssid crc check err\n", __func__, __LINE__);

		return 0;
	}

	src_data_buff[0] = 0;

	return 1;
}

/*save data*/
static int dec_push_data(struct ieee80211_frame *iframe, SC_Lead_Code *lead_code, uint8_t *src_data_buff)
{
	uint8_t packet_num = iframe->i_addr3[3];
	uint8_t *data = &iframe->i_addr3[4];
	int d = 0;

	packet_num -= 5;

	if (dec_valid_packet_count(lead_code->count_pkt, packet_num) == 1)
		lead_code->packet_count += 1;
	SC_DECODE_DBG(INFO, "push packet_num : %d\n", packet_num);

	*(src_data_buff + packet_num * 2 ) = *data;
	*(src_data_buff + packet_num * 2 + 1) = *(data + 1);
	d = lead_code->packet_count - lead_code->packet_sum;

	return d;
}

SMART_CONFIG_STATUS_T sc_dec_packet_deoced(SC_Priv *sc_priv, uint8_t *data)
{
	struct ieee80211_frame *iframe = (struct ieee80211_frame *)data;
	uint8_t packet_num = iframe->i_addr3[3];
	int ret;

	SC_BUG_ON(!sc_priv);

	if ((iframe->i_addr3[0] == SC_ADD0) && (iframe->i_addr3[1] == SC_ADD1) && \
	      (iframe->i_addr3[2]== SC_ADD2))
	      ;
	else
		return sc_priv->Sc_Status;

	if (!sc_priv->lead_code_inited) {
		memset(&sc_priv->lead_code , 0, sizeof(SC_Lead_Code));
		sc_priv->lead_code.pwd_size = -1;
		sc_priv->lead_code_inited = 1;
	}

	if (sc_priv->lead_code.lead_complete_flag == 0) {
		ret = dec_get_lead_code(sc_priv, iframe, &sc_priv->lead_code);
		if(ret == LEAD_CODE_COMPLETE)
			sc_priv->lead_code.lead_complete_flag = 1;
		return sc_priv->Sc_Status;
	}

	if (packet_num <= 4)
		return sc_priv->Sc_Status;

	if (sc_priv->lead_code.lead_complete_flag == 1 && packet_num <= 127) {
		if (dec_push_data(iframe, &sc_priv->lead_code, sc_priv->src_data_buff) != 0)
			return sc_priv->Sc_Status;
	}

	if (sc_priv->lead_code.packet_sum == sc_priv->lead_code.packet_count){
		SC_DECODE_DBG(INFO, "DATA IS ALL RECEIVE\n");
		SC_DECODE_DBG(INFO, "data_decode\n");
		int data_decode_ret = dec_data_decode(sc_priv, &sc_priv->sc_result, &sc_priv->lead_code, sc_priv->src_data_buff);
		if (data_decode_ret) {
			SC_DECODE_DBG(INFO, "decode success\n");
			sc_priv->Sc_Status = SC_COMPLETED;
		} else {
			SC_DECODE_DBG(INFO, "decode err\n");
			if (sc_priv->lead_code_inited) {
				sc_priv->lead_code_inited = 0;
			}

			if (sc_priv->src_data_buff[0]) {
				sc_priv->src_data_buff[0] = 0;
			}

			sc_priv->Sc_Status = SC_END;
		}
	}

	return sc_priv->Sc_Status;
}

int sc_set_aeskey(SC_Priv *sc_priv, const char *key, uint32_t length)
{
	if (length != SC_KEY_LEN)
		return -1;

	memcpy(sc_priv->Aes_Key, key, SC_KEY_LEN);

	return 0;
}

uint16_t sc_read_locked_channel(SC_Priv *sc_priv)
{
	return sc_priv->lead_code.channel;
}

SC_Result *sc_read_result(SC_Priv *sc_priv)
{
	return (sc_priv->sc_result.ssid[0] ? &sc_priv->sc_result : NULL);
}
