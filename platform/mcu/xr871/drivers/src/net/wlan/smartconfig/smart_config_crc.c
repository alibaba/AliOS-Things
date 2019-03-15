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

#include "smart_config_crc.h"
#include "driver/chip/hal_crypto.h"

#define CRC8_INIT       0x0
#define CRC8_POLY       0x31

unsigned char cal_crc8(const unsigned char *in, int num)
{
	unsigned char crc = CRC8_INIT;
	int i, j;

	for (i = 0; i < num; i++ ){
		crc ^= in[i];
		for(j = 0; j < 8; j++){
			if (crc & 0x1)
				crc = (crc >> 1) ^ CRC8_POLY;
			else
				crc = crc >> 1;
		}

	}

	return crc;
}

static int aes_decrypt(char *aes_key, char *enc_data, uint32_t enc_data_len, char *dec_buf)
{
	HAL_Status status = HAL_ERROR;
	CE_AES_Config aes_cfg;
	memset(&aes_cfg, 0, sizeof(aes_cfg));

	sprintf((char*)aes_cfg.key, aes_key);
	aes_cfg.keysize = CE_CTL_AES_KEYSIZE_128BITS;
	aes_cfg.mode = CE_CRYPT_MODE_ECB; //CBC;
	aes_cfg.src = CE_CTL_KEYSOURCE_INPUT;

	//HAL_CE_Init();

	status = HAL_AES_Decrypt(&aes_cfg, (uint8_t*)enc_data, (uint8_t*)dec_buf, enc_data_len);
	if (status != HAL_OK) {
		printf("AES decrypt error %d\n", status);
		return -1;
	}

	return 0;
}

static int aes_128_decrypt_block(char *key, char *in, int in_len, char *out)
{
	if (aes_decrypt(key, in, in_len, out) != 0) {
		printf("ce decrypt error\n");
		return -1;
	}

	return 0;
}

int aes_ebc_decrypt(char *in, char* out, int in_len, char *key)
{
	int ret = -1;

	ret = aes_128_decrypt_block(key, in, in_len, out);

	return ret;
}
