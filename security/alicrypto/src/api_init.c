/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

ali_crypto_result ali_crypto_init(void)
{
    return ALI_CRYPTO_SUCCESS;
}

void ali_crypto_cleanup(void)
{
    return;
}

ali_crypto_result ali_crypt_get_errcode(uint8_t code)
{
	ali_crypto_result result;
	
	switch((ls_hal_crypt_result)code) {
		case HAL_CRYPT_SUCCESS:
			result = ALI_CRYPTO_SUCCESS;
			break;
		case HAL_CRYPT_NOSUPPORT:
			result = ALI_CRYPTO_NOSUPPORT;
			break;
		case HAL_CRYPT_INVALID_CONTEXT:
			result = ALI_CRYPTO_INVALID_CONTEXT;
			break;
		case HAL_CRYPT_INVALID_ARG:
			result = ALI_CRYPTO_INVALID_ARG;
			break;
		case HAL_CRYPT_LENGTH_ERR:
			result = ALI_CRYPTO_LENGTH_ERR;
			break;
		case HAL_CRYPT_OUTOFMEM:
			result = ALI_CRYPTO_OUTOFMEM;
			break;
		case HAL_CRYPT_SHORT_BUFFER:
			result = ALI_CRYPTO_SHORT_BUFFER;
			break;
		default:
			result = ALI_CRYPTO_ERROR;
			break;
		}

	return result;
}