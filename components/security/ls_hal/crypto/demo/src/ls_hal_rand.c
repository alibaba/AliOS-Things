/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"
#include <sys/time.h>

static uint32_t randseed = 12345;

static uint32_t _crypt_rand_word(void)
{
    return (randseed = randseed * 1664525 + 1013904223);
}

static void _rand_gen(uint8_t *buf, size_t len)
{
    uint32_t i;
    uint32_t tmp;

    tmp = _crypt_rand_word();
    for (i = 0; i < len; i++) {
        if ((i & 3) == 0) {
            tmp = _crypt_rand_word();
        }
        buf[i] = ((tmp >> ((i & 3) << 3)) & 0xff);
    }
}

int ls_hal_get_random(uint8_t* buf, size_t len)
{
    if (buf == NULL || len == 0) {
        LS_HAL_LOG("invalid input args\n");
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    // gen rand
    _rand_gen(buf, len);

    return HAL_CRYPT_SUCCESS;
}
