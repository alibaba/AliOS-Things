/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_NOR_STM32L4_H
#define __HAL_NOR_STM32L4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"


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

#define NOR_HANDLE_INSTANCE     FMC_NORSRAM_DEVICE
#define NOR_HANDLE_EXTENDED     FMC_NORSRAM_EXTENDED_DEVICE

#define NOR_ADDR_SETUP_TIME     0x1
#define NOR_ADDR_HOLD_TIME      0x1
#define NOR_DATA_SETUP_TIME     0x6
#define NOR_BUS_TURN_DURATION   0x0
#define NOR_CLK_DIVISION        0x2
#define NOR_DATA_LATENCY        0x2
#define NOR_ACCESS_MODE         FMC_ACCESS_MODE_B

#define NOR_INIT_NSBANK             FMC_NORSRAM_BANK2
#define NOR_INIT_DATA_ADDR_MUX      FMC_DATA_ADDRESS_MUX_DISABLE
#define NOR_INIT_MEMORY_TYPE        FMC_MEMORY_TYPE_NOR
#define NOR_INIT_MEM_DATA_WIDTH     FMC_NORSRAM_MEM_BUS_WIDTH_16
#define NOR_INIT_BURST_ACCESS_MODE  FMC_BURST_ACCESS_MODE_DISABLE
#define NOR_INIT_WAIT_SIGNAL_PRI    FMC_WAIT_SIGNAL_POLARITY_LOW
#define NOR_INIT_WAIT_SIGNAL_ACT    FMC_WAIT_TIMING_BEFORE_WS
#define NOR_INIT_WRITE_OPERATION    FMC_WRITE_OPERATION_ENABLE
#define NOR_INIT_WAIT_SIGNAL        FMC_WAIT_SIGNAL_DISABLE
#define NOR_INIT_EXTENDED_MODE      FMC_EXTENDED_MODE_DISABLE
#define NOR_INIT_ASYNC_WAIT         FMC_ASYNCHRONOUS_WAIT_DISABLE
#define NOR_INIT_WRITE_BURST        FMC_WRITE_BURST_DISABLE
#define NOR_INIT_CONTINUOUS_CLK     FMC_CONTINUOUS_CLOCK_SYNC_ONLY
#define NOR_INIT_PAGE_SIZE          FMC_PAGE_SIZE_NONE

#ifdef __cplusplus
}
#endif

#endif /* __HAL_NOR_STM32L4_H */
