/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include "utils_sha256.h"

int pal_sha256_sum(const uint8_t *addr, const int len, uint8_t *mac, uint32_t *mac_len)
{
    utils_sha256(addr, (size_t)len, mac);
    *mac_len = 32;
    return 0;
}
