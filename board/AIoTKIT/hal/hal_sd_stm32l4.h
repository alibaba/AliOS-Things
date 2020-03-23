/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_SD_STM32L4_H
#define __HAL_SD_STM32L4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"


#if defined(STM32L496xx)
#include "stm32l496xx.h"
#endif

#define SD_INSTANCE             SDMMC1
#define SD_INIT_CLK_EDGE        SDMMC_CLOCK_EDGE_RISING
#define SD_INIT_CLK_BYPASS      SDMMC_CLOCK_BYPASS_DISABLE
#define SD_INIT_CLK_POWER_SAVE  SDMMC_CLOCK_POWER_SAVE_DISABLE
#define SD_INIT_BUS_WIDE        SDMMC_BUS_WIDE_1B
#define SD_INIT_HW_FLOW_CTRL    SDMMC_HARDWARE_FLOW_CONTROL_ENABLE
#define SD_INIT_CLK_DIVISION    SDMMC_TRANSFER_CLK_DIV

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SD_STM32L4_H */
