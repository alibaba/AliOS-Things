/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides method to find the least/most significant bit in 32 bit
fields.
*/

#include "k_api.h"
#include "k_ffs.h"
#include "k_bitmap.h"

/* find most significant bit set */

int ffs32_msb(uint32_t bitmap)
{
    if (bitmap == 0) {
        return 0;
    }

    return 32 - krhino_find_first_bit(&bitmap);
}


/* find least significant bit set */

int ffs32_lsb(uint32_t bitmap)
{
    uint32_t x;
    int      lsbit;

    if (bitmap == 0) {
        return 0;
    }

    x = bitmap & -bitmap;
    lsbit = krhino_find_first_bit((uint32_t *)(&x));

    return 32 - lsbit;
}

