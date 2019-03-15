/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ALG_H__
#define __ALG_H__

#include <stdint.h>
#include "error_code.h"

enum
{
	DES_BLOCK_SIZE	= 0x08,
	AES_BLOCK_SIZE	= 0x10,
};

irot_result_t des3_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc);

irot_result_t aes_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc);

#endif
