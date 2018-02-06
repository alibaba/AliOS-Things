/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_ADDR_MAP_H_
#define _TEE_ADDR_MAP_H_

#include "hal_memmap.h"

/*****************************************/
/* Memory Map for Sillicon               */
/*****************************************/

/*******************************************************************/
/*  TEE TW address map                                             */
/*******************************************************************/
#define TW_RO_ADDR          (FLASH_START)
#define TW_RO_SIZE          (HAL_TW_RO_SIZE)
#define NTW_ENTRY_ADDR      (TW_RO_ADDR + TW_RO_SIZE)
#define TW_RW_ADDR          (SRAM_START)
#define TW_RW_SIZE          (HAL_TW_RW_SIZE)
#define TEE_HEAP_SIZE       (_SIZE_4K)

/*******************************************************************/
/*  TEE NTW(test bed) address map                                  */
/*******************************************************************/
#define NTW_RO_ADDR         (FLASH_START  + TW_RO_SIZE)
#define NTW_RO_SIZE         (HAL_NTW_RO_SIZE)
#define NTW_RW_ADDR         (SRAM_START + _SIZE_32K)
#define NTW_RW_SIZE         (HAL_NTW_RW_SIZE)
#define NTW_HEAP_SIZE       (_SIZE_8K)

#endif /* _TEE_ADDR_MAP_H_ */
