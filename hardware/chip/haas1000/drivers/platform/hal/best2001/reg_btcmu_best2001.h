/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_BTCMU_BEST2001_H__
#define __REG_BTCMU_BEST2001_H__

#include "plat_types.h"

struct BTCMU_T {
    __IO uint32_t CLK_ENABLE;           // 0x00
    __IO uint32_t CLK_DISABLE;          // 0x04
    __IO uint32_t CLK_MODE;             // 0x08
    __IO uint32_t DIV_TIMER;            // 0x0C
    __IO uint32_t RESET_SET;            // 0x10
    __IO uint32_t RESET_CLR;            // 0x14
    __IO uint32_t DIV_WDT;              // 0x18
    __IO uint32_t RESET_PULSE;          // 0x1C
         uint32_t RESERVED_020[0x24 / 4]; // 0x20
    __IO uint32_t CLK_OUT;              // 0x44
         uint32_t RESERVED_048[2];      // 0x48
    __IO uint32_t ISIRQ_SET;            // 0x50
    __IO uint32_t ISIRQ_CLR;            // 0x54
};

// reg_44
#define BT_CMU_CAL_TIME(n)                      (((n) & 0xFF) << 0)
#define BT_CMU_CAL_TIME_MASK                    (0xFF << 0)
#define BT_CMU_CAL_TIME_SHIFT                   (0)
#define BT_CMU_SMP_CMU_SEL(n)                   (((n) & 0xF) << 8)
#define BT_CMU_SMP_CMU_SEL_MASK                 (0xF << 8)
#define BT_CMU_SMP_CMU_SEL_SHIFT                (8)
#define BT_CMU_CFG_CLK_OUT(n)                   (((n) & 0xF) << 12)
#define BT_CMU_CFG_CLK_OUT_MASK                 (0xF << 12)
#define BT_CMU_CFG_CLK_OUT_SHIFT                (12)
#define BT_CMU_PWR_DELAY(n)                     (((n) & 0x7F) << 16)
#define BT_CMU_PWR_DELAY_MASK                   (0x7F << 16)
#define BT_CMU_PWR_DELAY_SHIFT                  (16)
#define BT_CMU_ROM_PGEN(n)                      (((n) & 0xF) << 23)
#define BT_CMU_ROM_PGEN_MASK                    (0xF << 23)
#define BT_CMU_ROM_PGEN_SHIFT                   (23)
#define BT_CMU_RAM_EMAS                         (1 << 27)
#define BT_CMU_RF_EMAS                          (1 << 28)

#endif

