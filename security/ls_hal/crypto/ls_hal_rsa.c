/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"

int ls_hal_rsa_public(rsa_pub_key_t *pub_key,
           const uint8_t *src, uint8_t *dst, size_t size)
{
    (void)pub_key;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_rsa_private(rsa_priv_key_t *priv_key,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size)
{
    (void)priv_key;
    (void)f_rng;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}
