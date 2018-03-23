/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_NOR_STM32L4_H
#define __HAL_NOR_STM32L4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "hal/soc/soc.h"

#if defined(STM32L496xx)
#include "stm32l496xx.h"
#endif

#define NOR_DEVICE_ADDR         ((uint32_t)0x64000000)

/* NOR size configuration */
#define NOR_BLOCK_SIZE          0x1000
#define NOR_CHIP_SIZE

/* NOR operations Timeout definitions */
#define BLOCKERASE_TIMEOUT      2000U  /* NOR block erase timeout : 2 s */
#define CHIPERASE_TIMEOUT       400000U  /* NOR chip erase timeout : 400 s */
#define PROGRAM_TIMEOUT         13U  /* NOR program timeout : 64 bytes * 200 us = 12.8 ms */



#ifdef __cplusplus
}
#endif

#endif /* __HAL_NOR_STM32L4_H */
