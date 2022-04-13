/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_TRNG_H__
#define __REG_TRNG_H__

#include "plat_types.h"

struct TRNG_T
{
	uint8 reserved1[0x100];           //0x0-0xfc
    __IO uint32_t RNG_IMR;            // 0x100
    __I  uint32_t RNG_ISR;            // 0x104
    __O  uint32_t RNG_ICR;            // 0x108
    __IO uint32_t TRNG_CONFIG;        // 0x10C
    __I  uint32_t TRNG_VALID;         //0x110
    __I  uint32_t EHR_DATA[6];        //0x114-0x128
    __IO uint32_t RND_SOURCE_ENABLE;  //0x12c
    __IO uint32_t SAMPLE_CNT1;        //0x130
    __IO uint32_t AUTOCORR_STATISTIC; //0x134
    __I  uint32_t TRNG_DEBUG_CONTROL; //0x138
    uint8 reserved2[0x4];             //0x13c
    __O  uint32_t TRNG_SW_RESET;      //0x140
    uint8 reserved3[0x70];            //0x144-0x1b4
    __I  uint32_t TRNG_BUSY;          //0x1b8
    __O  uint32_t RST_BITS_COUNTER;   //0x1bc
    uint8 reserved4[0x1c];            //0x1c0-0x1dc
    __I  uint32_t RNG_BIST_CNTR[3];   //0x1e0-0x1e8
};


#define RND_SOURCE_EN    (1 << 0)
#define TRNG_VALID_BIT   (1 << 0)
#define TRNG_BUSY_BIT    (1 << 0)
#define RND_SRC_SEL_MASK (0x3)

#endif
