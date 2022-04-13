/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_RTC_H__
#define __REG_RTC_H__

#include "plat_types.h"

// PL031 Registers
struct RTC_T
{
    __I  uint32_t RTCDR;            // 0x000
    __IO uint32_t RTCMR;            // 0x004
    __IO uint32_t RTCLR;            // 0x008
    __IO uint32_t RTCCR;            // 0x00C
    __IO uint32_t RTCIMSC;          // 0x010
    __I  uint32_t RTCRIS;           // 0x014
    __I  uint32_t RTCMIS;           // 0x018
    __O  uint32_t RTCICR;           // 0x01C
};

#define RTC_CR_EN    (1 << 0)    /* counter enable bit */

#define RTC_BIT_AI    (1 << 0) /* Alarm interrupt bit */

#endif
