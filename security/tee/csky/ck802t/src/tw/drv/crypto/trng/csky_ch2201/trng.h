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
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 **/

#ifndef TRNG_H
#define TRNG_H
#include "tee_types.h"

#define TRNG_BASEADDR 0x40009000
#define TRNG_TCR 0x0
#define TRNG_TDR 0x4

#define TRNG_EN 2
#define TRNG_LOWPER_MODE 4


static inline void trng_enable(void)
{
    *(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) |= TRNG_EN;
}

static inline void trng_disable(void)
{
    *(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) &= ~TRNG_EN;
}

static inline void trng_lowpermode_enable(void)
{
    *(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) |= TRNG_LOWPER_MODE;
}

static inline void trng_lowpermode_disable(void)
{
    *(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) &= ~TRNG_LOWPER_MODE;
}

static inline uint32_t trng_data_ready_status(void)
{
    return (*(volatile uint32_t *)(TRNG_BASEADDR + TRNG_TCR) & 0x1);
}

int32_t trng_random_get(void *arg);

#endif
