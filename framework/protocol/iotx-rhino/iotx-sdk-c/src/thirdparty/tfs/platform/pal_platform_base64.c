/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include "utils_base64.h"

void pal_base64_encode(const unsigned char *src, int len,
                       unsigned char *dst, int *out_len)
{
    utils_base64encode(src, (uint32_t)len, (uint32_t)len * 5, dst, (uint32_t *)out_len);
    return;
}

