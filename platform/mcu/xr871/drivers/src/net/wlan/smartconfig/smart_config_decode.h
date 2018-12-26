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

#ifndef __SC_DECODE_H__
#define __SC_DECODE_H__

#include "types.h"

#define SC_ADD0 0x01
#define SC_ADD1 0x00
#define SC_ADD2 0X5E

typedef enum {
	SC_VALUE_FALSE = 0,
	SC_VALUE_TRUE = 1,
} SC_BOOL;

typedef enum {
	SC_VALUE_NOT_READY,
	SC_VALUE_COMPLETE,
} SC_RESULT_STA;

typedef enum {
	SC_END = 0,
	SC_SEARCH_CHAN,
	SC_LOCKED_CHAN,
	SC_COMPLETED,
} SMART_CONFIG_STATUS_T;

typedef struct {
	uint8_t ssid[65];
	uint8_t pwd[66];
	uint8_t ssid_size ;
	uint8_t pwd_size ;
	uint8_t random_num;
} SC_Result;

typedef struct {
	int channel;
	int ssidpwd_size;
	int pwd_size;
	int packet_sum;
	int packet_count;
	uint8_t random_num;
	uint8_t lead_complete_flag;
	uint8_t count_pkt[16];
} SC_Lead_Code;

#define SC_KEY_LEN 16

typedef struct {
	char Aes_Key[SC_KEY_LEN];
	SC_Lead_Code lead_code;
	int lead_code_inited;
	SMART_CONFIG_STATUS_T sc_ctrl;
	uint8_t locked_mac[6];
	int locked_mac_flag;
	SMART_CONFIG_STATUS_T Sc_Status;
	SC_Result sc_result;
	uint8_t src_data_buff[1500];
} SC_Priv;

SMART_CONFIG_STATUS_T sc_dec_packet_deoced(SC_Priv *sc_priv, uint8_t *data);
SC_Result *sc_read_result(SC_Priv *sc_priv);
uint16_t sc_read_locked_channel(SC_Priv *sc_priv);
/* the key length must be 16 byte */
int sc_set_aeskey(SC_Priv *sc_priv, const char *key, uint32_t length);

#endif /* __SC_DECODE_H__ */
