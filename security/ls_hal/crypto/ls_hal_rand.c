/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"

int ls_hal_get_random(uint8_t* buf, size_t len)
{
    (void)buf;
    (void)len;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}
