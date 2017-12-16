/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************/
/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 **/
#include "tee_types.h"
#include "tee_dbg.h"
#include "trng.h"
#include "device.h"
#include "crypto_drv.h"

static uint32_t trng_data_get(void)
{
    uint32_t data = 0;
    int      cnt = 200;

    trng_enable();

    do {
        if (*(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) & 0x1) {
            data = *(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TDR);
            break;
        }
    } while (cnt--);

    trng_disable();

    return data;
}

int32_t trng_random_get(void *arg)
{
    trng_drv_ioctl_t *p = (trng_drv_ioctl_t *)arg;
    uint8_t *data;
    uint32_t len;
    uint32_t i;
    uint32_t tmp;

    if (NULL == p) {
        return EARG;
    }

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    data = p->buf;
    len = p->len;

    for (i = 0; i < len; i+=sizeof(uint32_t)) {
        tmp = trng_data_get();
        memcpy((void *)(data+i), (void *)&tmp, sizeof(uint32_t) > (len - i) ? len - i : sizeof(uint32_t));
    }

    return SUCCESS;
}

int32_t trng_drv_suspend(dev_t *dev, int32_t state)
{
    return SUCCESS;
}

int32_t trng_drv_resume(dev_t *dev)
{
    return SUCCESS;
}
