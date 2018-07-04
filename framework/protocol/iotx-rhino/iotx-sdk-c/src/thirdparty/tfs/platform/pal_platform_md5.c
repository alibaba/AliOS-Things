/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include "utils_md5.h"

void pal_md5_sum(const uint8_t *addr, const int len, uint8_t *mac)
{
    utils_md5(addr, (size_t)len, mac);
    return;
}
