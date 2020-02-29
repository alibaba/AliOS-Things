/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __LS_HAL_CRYPT_H__
#define __LS_HAL_CRYPT_H__

#include "ls_osa.h"
#include "ls_hal_rsa.h"
#include "ls_hal_aes.h"
#include "ls_hal_hash.h"

/* HAL Crypto Error Code */
typedef enum _hal_crypto_result {
    HAL_CRYPT_ERROR = (int)0xFFFF0000,     /* Generic Error */
    HAL_CRYPT_BAD_PARAMETERS,              /* Bad Parameters */
    HAL_CRYPT_NOSUPPORT,                   /* Scheme not support */
    HAL_CRYPT_INVALID_CONTEXT,             /* Invalid context */
    HAL_CRYPT_INVALID_ARG,                 /* Invalid argument */
    HAL_CRYPT_LENGTH_ERR,                  /* Invalid Length in arguments */
    HAL_CRYPT_OUTOFMEM,                    /* Memory alloc NULL */
    HAL_CRYPT_SHORT_BUFFER,                /* Output buffer is too short to store result */
    HAL_CRYPT_SUCCESS = 0,                 /* Success */
} ls_hal_crypt_result;

/*
 * Generate random data with len bytes
 *
 * buf[in/out]:  buffer to store the results
 * len[in]:      size of buffer
 */
int ls_hal_get_random(uint8_t* buf, size_t len);

#endif /*__LS_HAL_CRYPT_H__ */
