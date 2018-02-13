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
#include "cmd_ce.h"
#include "driver/chip/hal_crypto.h"

#define CE_TEST_KEYNUM 5

#define CE_TEST_KEYMAXSIZE sizeof(CE_AES_Config)

typedef enum {
	NONE,
	AES,
	DES,
	TDES
} crypto_key_type;

typedef struct {
	crypto_key_type type;
	void *key_cfg;
} crypto_key;

static crypto_key key[CE_TEST_KEYNUM];
static uint32_t srv_cnt;

/*static CE_CRC_Handler crc;
static CE_MD5_Handler md5;
static CE_SHA1_Handler sha1;
static CE_SHA256_Handler sha256;

uint32_t crc_cont;
uint32_t md5_cont;
uint32_t sha1_cont;
uint32_t sha256_cont;*/

static uint32_t md5_iv[4];
static uint32_t sha1_iv[5];
static uint32_t sha256_iv[8];

static enum cmd_status cmd_crypto_addkey_exec(char *cmd)
{
	int32_t cnt;
	char type_str[8];
	char mode_str[8];
	uint32_t i = 0;
	uint32_t keysize;
	CE_Crypto_Mode mode;
	void *p;
	uint8_t buf[32];
	uint32_t keysrc;

	/* get param */
	cnt = cmd_sscanf(cmd, "t=%7s m=%7s k=%u", type_str, mode_str, &keysrc);

	/* check param */
	if (cnt != 3) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (keysrc > 10) {
		CMD_ERR("invalid keysrc %d\n", keysrc);
		return CMD_STATUS_INVALID_ARG;
	}

	/* create key */
	while (key[i].type != NONE) {
		i++;
		if (i > CE_TEST_KEYNUM) {
			CMD_ERR("key full\n");
			return CMD_STATUS_FAIL;
		}
	}

	key[i].key_cfg = cmd_malloc(CE_TEST_KEYMAXSIZE);	//	staic may be good.
	p = key[i].key_cfg;
	if (p == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	/* check param */
	if (cmd_strcmp(mode_str, "ecb") == 0) {
		mode = CE_CRYPT_MODE_ECB;
	} else if (cmd_strcmp(mode_str, "cbc") == 0) {
		mode = CE_CRYPT_MODE_CBC;
	} else {
		cmd_free(key[i].key_cfg);
		CMD_ERR("invalid mode %s\n", mode_str);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(type_str, "aes128") == 0) {
		keysize = 16;
		key[i].type = AES;
		((CE_AES_Config *)p)->mode = mode;
		((CE_AES_Config *)p)->src = (CE_Crypto_KeySrc)keysrc;
		((CE_AES_Config *)p)->keysize = CE_CTL_AES_KEYSIZE_128BITS;
	} else if (cmd_strcmp(type_str, "aes192") == 0) {
		keysize = 24;
		key[i].type = AES;
		((CE_AES_Config *)p)->mode = mode;
		((CE_AES_Config *)p)->src = (CE_Crypto_KeySrc)keysrc;
		((CE_AES_Config *)p)->keysize = CE_CTL_AES_KEYSIZE_192BITS;
	} else if (cmd_strcmp(type_str, "aes256") == 0) {
		keysize = 32;
		key[i].type = AES;
		((CE_AES_Config *)p)->mode = mode;
		((CE_AES_Config *)p)->src = (CE_Crypto_KeySrc)keysrc;
		((CE_AES_Config *)p)->keysize = CE_CTL_AES_KEYSIZE_256BITS;
	} else if (cmd_strcmp(type_str, "des") == 0) {
		keysize = 8;
		key[i].type = DES;
		((CE_DES_Config *)p)->mode = mode;
		((CE_DES_Config *)p)->src = (CE_Crypto_KeySrc)keysrc;
	} else if (cmd_strcmp(type_str, "3des") == 0) {
		keysize = 24;
		key[i].type = TDES;
		((CE_3DES_Config *)p)->mode = mode;
		((CE_3DES_Config *)p)->src = (CE_Crypto_KeySrc)keysrc;
	} else {
		cmd_free(key[i].key_cfg);
		CMD_ERR("invalid type %s\n", type_str);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", i);

	if (keysrc == 0) {

		cmd_raw_mode_enable();
		cnt = cmd_raw_mode_read(buf, keysize, 30000);
		cmd_raw_mode_disable();

		buf[cnt] = 0;

		switch (key[i].type) {
		case AES:
			cmd_memcpy(((CE_AES_Config *)p)->key, buf, keysize);
			break;
		case DES:
			cmd_memcpy(((CE_DES_Config *)p)->key, buf, keysize);
			break;
		case TDES:
			cmd_memcpy(((CE_3DES_Config *)p)->key, buf, keysize);
			break;
		default:
			break;
		}
	}

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_crypto_delkey_exec(char *cmd)
{
	int32_t cnt;
	uint32_t i;

	/* get param */
	cnt = cmd_sscanf(cmd, "k=%u", &i);

	/* check param */
	if (cnt != 1) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((i >= CE_TEST_KEYNUM) || (key[i].type == NONE)) {
		CMD_ERR("invalid key %d\n", i);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_free(key[i].key_cfg);
	key[i].type = NONE;

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_crypto_keyiv_exec(char *cmd)
{
	int32_t cnt;
	uint32_t i;
	uint32_t size;
	void *p;
	uint8_t buf[16];
	uint32_t iv_size;


	/* get param */
	cnt = cmd_sscanf(cmd, "k=%u s=%u", &i, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((i >= CE_TEST_KEYNUM) || (key[i].type == NONE)) {
		CMD_ERR("invalid key %d\n", i);
		return CMD_STATUS_INVALID_ARG;
	}

	if (key[i].type == AES) {
		iv_size = 16;
		if (size > 16) {
			CMD_ERR("invalid size %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
	} else {
		iv_size = 8;
		if (size > 8) {
			CMD_ERR("invalid size %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
	}

	p = key[i].key_cfg;

	cmd_write_respond(CMD_STATUS_OK, "%d", iv_size);

	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, iv_size, 30000);
	cmd_raw_mode_disable();

	switch (key[i].type) {
	case AES:
		cmd_memset(((CE_AES_Config *)p)->iv, 0, iv_size);
		cmd_memcpy(((CE_AES_Config *)p)->iv, buf, size);
		break;
	case DES:
		cmd_memset(((CE_DES_Config *)p)->iv, 0, iv_size);
		cmd_memcpy(((CE_DES_Config *)p)->iv, buf, size);
		break;
	case TDES:
		cmd_memset(((CE_3DES_Config *)p)->iv, 0, iv_size);
		cmd_memcpy(((CE_3DES_Config *)p)->iv, buf, size);
		break;
	default:
		break;
	}

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_crypto_enc_exec(char *cmd)
{
	int32_t cnt;
	uint32_t keyi;
	uint32_t size;
	uint32_t enc_size;
	uint8_t *plain;
	uint8_t *cipher;

	/* get param */
	cnt = cmd_sscanf(cmd, "k=%u s=%u", &keyi, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((keyi >= CE_TEST_KEYNUM) || (key[keyi].type == NONE)) {
		CMD_ERR("invalid key %d\n", keyi);
		return CMD_STATUS_INVALID_ARG;
	}

	plain = cmd_malloc(size);
	if (plain == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cipher = cmd_malloc(size + 16);
	if (cipher == NULL) {
		CMD_ERR("no memory\n");
		cmd_free(plain);
		return CMD_STATUS_FAIL;
	}

	if (key[keyi].type == AES)
		enc_size = (size + 15) & (~0xF);
	else
		enc_size = (size + 7) & (~0x7);

	cmd_write_respond(CMD_STATUS_OK, "%d", enc_size);
	cmd_raw_mode_enable();
	cmd_raw_mode_read(plain, size, 30000);

	switch (key[keyi].type) {
	case AES:
		HAL_AES_Encrypt((CE_AES_Config *)(key[keyi].key_cfg), plain, cipher, size);
		break;
	case DES:
		HAL_DES_Encrypt((CE_DES_Config *)(key[keyi].key_cfg), plain, cipher, size);
		break;
	case TDES:
		HAL_3DES_Encrypt((CE_3DES_Config *)(key[keyi].key_cfg), plain, cipher, size);
		break;
	default:
		break;
	}

	cmd_raw_mode_write(cipher, enc_size);
	cmd_raw_mode_disable();

	cmd_free(plain);
	cmd_free(cipher);
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_crypto_dec_exec(char *cmd)
{
	int32_t cnt;
	uint32_t keyi;
	uint32_t size;
	uint8_t *plain;
	uint8_t *cipher;

	/* get param */
	cnt = cmd_sscanf(cmd, "k=%u s=%u", &keyi, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((keyi >= CE_TEST_KEYNUM) || (key[keyi].type == NONE)) {
		CMD_ERR("invalid key %d\n", keyi);
		return CMD_STATUS_INVALID_ARG;
	}

	if (key[keyi].type == AES) {
		if (size & 0xF) {
			CMD_ERR("invalid size %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
	 } else {
		if (size & 0x7) {
			CMD_ERR("invalid size %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
	 }

	plain = cmd_malloc(size);
	if (plain == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cipher = cmd_malloc(size + 16);
	if (cipher == NULL) {
		CMD_ERR("no memory\n");
		cmd_free(plain);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", size);
	cmd_raw_mode_enable();
	cmd_raw_mode_read(cipher, size, 30000);

	switch (key[keyi].type) {
	case AES:
		HAL_AES_Decrypt((CE_AES_Config *)(key[keyi].key_cfg), cipher, plain, size);
		break;
	case DES:
		HAL_DES_Decrypt((CE_DES_Config *)(key[keyi].key_cfg), cipher, plain, size);
		break;
	case TDES:
		HAL_3DES_Decrypt((CE_3DES_Config *)(key[keyi].key_cfg), cipher, plain, size);
		break;
	default:
		break;
	}

	cmd_raw_mode_write(plain, size);
	cmd_raw_mode_disable();

	cmd_free(plain);
	cmd_free(cipher);
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_crypto_crc_exec(char *cmd)
{
	char type_str[16];
	CE_CRC_Types type;
	uint32_t size;

	uint8_t *buf;
	CE_CRC_Handler crc;
	uint32_t half;
	uint32_t res;

	/* get param */
	cmd_sscanf(cmd, "t=%16s s=%u", type_str, &size);

	/* check param */
	if (cmd_strcmp(type_str, "ibm") == 0) {
		type = CE_CRC16_IBM;
	} else if (cmd_strcmp(type_str, "maxim") == 0) {
		type = CE_CRC16_MAXIM;
	} else if (cmd_strcmp(type_str, "usb") == 0) {
		type = CE_CRC16_USB;
	} else if (cmd_strcmp(type_str, "modbus") == 0) {
		type = CE_CRC16_MODBUS;
	} else if (cmd_strcmp(type_str, "ccitt_1") == 0) {
		type = CE_CRC16_CCITT_1;
	} else if (cmd_strcmp(type_str, "ccitt") == 0) {
		type = CE_CRC16_CCITT;
	} else if (cmd_strcmp(type_str, "x25") == 0) {
		type = CE_CRC16_X25;
	} else if (cmd_strcmp(type_str, "xmodem") == 0) {
		type = CE_CRC16_XMODEM;
	} else if (cmd_strcmp(type_str, "dnp") == 0) {
		type = CE_CRC16_DNP;
	} else if (cmd_strcmp(type_str, "crc32") == 0) {
		type = CE_CRC32;
	} else if (cmd_strcmp(type_str, "mpeg2") == 0) {
		type = CE_CRC32_MPEG2;
	} else {
		CMD_ERR("invalid type %s\n", type_str);
		return CMD_STATUS_INVALID_ARG;
	}

	half = size / 2;
	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", sizeof(res));

	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	if (HAL_CRC_Init(&crc, type, size) != HAL_OK) {
		CMD_ERR("crc init failed\n");
		return CMD_STATUS_FAIL;
	}
	if ((half) && (HAL_CRC_Append(&crc, buf, half) != HAL_OK)) {
		CMD_ERR("crc append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_CRC_Append(&crc, buf + half, size - half) != HAL_OK) {
		CMD_ERR("crc append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_CRC_Finish(&crc, &res) != HAL_OK) {
		CMD_ERR("crc deinit failed\n");
		return CMD_STATUS_FAIL;
	}

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	cmd_raw_mode_write((uint8_t *)&res, sizeof(res));
	cmd_raw_mode_disable();

	cmd_free(buf);
	return CMD_STATUS_ACKED;
}


static enum cmd_status cmd_crypto_md5_exec(char *cmd)
{
	char ivsrc_str[8];
	CE_Hash_IVsrc ivsrc;
	uint32_t size;

	uint8_t *buf;
	CE_MD5_Handler md5;
	uint32_t half;
	uint32_t res[4];

	/* get param */
	cmd_sscanf(cmd, "i=%7s s=%u", ivsrc_str, &size);

	/* check param */
	if (cmd_strcmp(ivsrc_str, "fips180") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_FIPS180;
	} else if (cmd_strcmp(ivsrc_str, "input") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_INPUT;
	} else {
		CMD_ERR("invalid ivsrc %s\n", ivsrc_str);
		return CMD_STATUS_INVALID_ARG;
	}

	half = size / 2;
	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", sizeof(res));

	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	if (HAL_MD5_Init(&md5, ivsrc, md5_iv) != HAL_OK) {
		CMD_ERR("md5 init failed\n");
		return CMD_STATUS_FAIL;
	}
	if ((half) && (HAL_MD5_Append(&md5, buf, half) != HAL_OK)) {
		CMD_ERR("md5 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_MD5_Append(&md5, buf + half, size - half) != HAL_OK) {
		CMD_ERR("md5 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_MD5_Finish(&md5, res) != HAL_OK) {
		CMD_ERR("md5 deinit failed\n");
		return CMD_STATUS_FAIL;
	}

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	cmd_raw_mode_write((uint8_t *)&res, sizeof(res));
	cmd_raw_mode_disable();

	cmd_free(buf);
	return CMD_STATUS_ACKED;

}


static enum cmd_status cmd_crypto_sha1_exec(char *cmd)
{
	char ivsrc_str[8];
	CE_Hash_IVsrc ivsrc;
	uint32_t size;

	uint8_t *buf;
	CE_SHA1_Handler sha1;
	uint32_t half;
	uint32_t res[5];

	/* get param */
	cmd_sscanf(cmd, "i=%7s s=%u", ivsrc_str, &size);

	/* check param */
	if (cmd_strcmp(ivsrc_str, "fips180") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_FIPS180;
	} else if (cmd_strcmp(ivsrc_str, "input") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_INPUT;
	} else {
		CMD_ERR("invalid ivsrc %s\n", ivsrc_str);
		return CMD_STATUS_INVALID_ARG;
	}

	half = size / 2;
	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", sizeof(res));

	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	if (HAL_SHA1_Init(&sha1, ivsrc, sha1_iv) != HAL_OK) {
		CMD_ERR("sha1 init failed\n");
		return CMD_STATUS_FAIL;
	}
	if ((half) && (HAL_SHA1_Append(&sha1, buf, half) != HAL_OK)) {
		CMD_ERR("sha1 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_SHA1_Append(&sha1, buf + half, size - half) != HAL_OK) {
		CMD_ERR("sha1 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_SHA1_Finish(&sha1, res) != HAL_OK) {
		CMD_ERR("sha1 deinit failed\n");
		return CMD_STATUS_FAIL;
	}

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	cmd_raw_mode_write((uint8_t *)&res, sizeof(res));
	cmd_raw_mode_disable();

	cmd_free(buf);
	return CMD_STATUS_ACKED;

}

static enum cmd_status cmd_crypto_sha256_exec(char *cmd)
{
	char ivsrc_str[8];
	CE_Hash_IVsrc ivsrc;
	uint32_t size;

	uint8_t *buf;
	CE_SHA256_Handler sha256;
	uint32_t half;
	uint32_t res[8];

	/* get param */
	cmd_sscanf(cmd, "i=%7s s=%u", ivsrc_str, &size);

	/* check param */
	if (cmd_strcmp(ivsrc_str, "fips180") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_FIPS180;
	} else if (cmd_strcmp(ivsrc_str, "input") == 0) {
		ivsrc = CE_CTL_IVMODE_SHA_MD5_INPUT;
	} else {
		CMD_ERR("invalid ivsrc %s\n", ivsrc_str);
		return CMD_STATUS_INVALID_ARG;
	}

	half = size / 2;
	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "%d", sizeof(res));

	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	if (HAL_SHA256_Init(&sha256, ivsrc, sha256_iv) != HAL_OK) {
		CMD_ERR("sha256 init failed\n");
		return CMD_STATUS_FAIL;
	}
	if ((half) && (HAL_SHA256_Append(&sha256, buf, half) != HAL_OK)) {
		CMD_ERR("sha256 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_SHA256_Append(&sha256, buf + half, size - half) != HAL_OK) {
		CMD_ERR("sha256 append failed\n");
		return CMD_STATUS_FAIL;
	}
	if (HAL_SHA256_Finish(&sha256, res) != HAL_OK) {
		CMD_ERR("sha256 deinit failed\n");
		return CMD_STATUS_FAIL;
	}

	cmd_raw_mode_write((uint8_t *)&res, sizeof(res));
	cmd_raw_mode_disable();

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	cmd_free(buf);
	return CMD_STATUS_ACKED;

}


static enum cmd_status cmd_crypto_prng_exec(char *cmd)
{
	uint8_t *res;
	uint32_t size;

	/* get param */
	cmd_sscanf(cmd, "s=%u", &size);

	(void)size;	//----------------------------the interface of prng should set size

	res = cmd_malloc(size);
	if (res == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	if (srv_cnt++ == 0)
		HAL_CE_Init();

	if (HAL_PRNG_Generate(res, size) != HAL_OK) {
		CMD_ERR("prng generate failed\n");
		return CMD_STATUS_FAIL;
	}

	if (--srv_cnt == 0)
		HAL_CE_Deinit();

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	cmd_msleep(1000);
	cmd_raw_mode_write((uint8_t *)res, size);
	cmd_raw_mode_disable();

	cmd_free(res);

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_crypto_digiv_exec(char *cmd)
{
	char type[8];
	uint32_t size;
	uint8_t *p;

	/* get param */
	cmd_sscanf(cmd, "t=%7s s=%u", type, &size);

	/* check param */
	if (cmd_strcmp(type, "md5") == 0) {
		if (size > 4 * 4) {
			CMD_ERR("invalid ivsize %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
		p = (uint8_t *)&md5_iv;
		cmd_memset(p, 0, 16);
	} else if (cmd_strcmp(type, "sha1") == 0) {
		if (size > 5 * 4) {
			CMD_ERR("invalid ivsize %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
		p = (uint8_t *)&sha1_iv;
		cmd_memset(p, 0, 20);
	}  else if (cmd_strcmp(type, "sha256") == 0) {
		if (size > 8 * 4) {
			CMD_ERR("invalid ivsize %d\n", size);
			return CMD_STATUS_INVALID_ARG;
		}
		p = (uint8_t *)&sha256_iv;
		cmd_memset(p, 0, 32);
	} else {
		CMD_ERR("invalid type %s\n", type);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	cmd_raw_mode_read(p, size, 30000);
	cmd_raw_mode_disable();

	return CMD_STATUS_ACKED;
}

static struct cmd_data g_crypto_cmds[] = {
	{ "addkey", 	cmd_crypto_addkey_exec },
	{ "delkey",	cmd_crypto_delkey_exec },
	{ "keyiv",	cmd_crypto_keyiv_exec },
	{ "enc",	cmd_crypto_enc_exec },
	{ "dec",	cmd_crypto_dec_exec },
	{ "crc",	cmd_crypto_crc_exec },
	{ "md5",	cmd_crypto_md5_exec },
	{ "sha1",	cmd_crypto_sha1_exec },
	{ "sha256",	cmd_crypto_sha256_exec },
	{ "prng",	cmd_crypto_prng_exec },
	{ "digiv",	cmd_crypto_digiv_exec },
};

enum cmd_status cmd_crypto_exec(char *cmd)
{
	return cmd_exec(cmd, g_crypto_cmds, cmd_nitems(g_crypto_cmds));
}
