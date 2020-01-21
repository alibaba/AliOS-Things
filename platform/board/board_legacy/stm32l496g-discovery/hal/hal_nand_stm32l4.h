/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_NAND_STM32L4_H
#define __HAL_NAND_STM32L4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"


#if defined(STM32L496xx)
#include "stm32l496xx.h"
#endif

#define NAND_HANDLE_INSTANCE        FMC_NAND_DEVICE

#define NAND_INIT_NAND_BANK         FMC_NAND_BANK3
#define NAND_INIT_WAIT_FEATURE      FMC_NAND_WAIT_FEATURE_ENABLE
#define NAND_INIT_MEMORY_DATA_WIDTH FMC_NAND_MEM_BUS_WIDTH_8
#define NAND_INIT_ECC_COMPUTATION   FMC_NAND_ECC_ENABLE
#define NAND_INIT_ECC_PAGE_SIZE     FMC_NAND_ECC_PAGE_SIZE_512BYTE
#define NAND_INIT_TCLR_SETUP_TIME   0
#define NAND_INIT_TAR_SETUP_TIME    0

#define NAND_INFO_PAGE_SIZE         0x800
#define NAND_INFO_SPARE_AREA_SIZE   0x40
#define NAND_INFO_BLOCK_SIZE        0x40
#define NAND_INFO_BLOCK_NBR         0x400
#define NAND_INFO_ZONE_SIZE         0x400

#define COM_SPACE_SETUP_TIME        0x5
#define COM_SPACE_WAIT_TIME         0x4
#define COM_SPACE_HOLD_TIME         0x2
#define COM_SPACE_HIZ_TIME          0x5

#define ATT_SPACE_SETUP_TIME        0x5
#define ATT_SPACE_WAIT_TIME         0x4
#define ATT_SPACE_HOLD_TIME         0x2
#define ATT_SPACE_HIZ_TIME          0x5

#ifdef __cplusplus
}
#endif

#endif /* __HAL_NAND_STM32L4_H */

