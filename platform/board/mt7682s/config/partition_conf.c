/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "aos/hal/flash.h"

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
  [HAL_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0x2000,
    .partition_length          = 0xFFFF,  //64kb  
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_APPLICATION] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "Application",
    .partition_start_addr      = 0x12000,
    .partition_length          = 0xD8000,   //680k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },

  [HAL_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = HAL_FLASH_NONE,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x0,
    .partition_length          = 0x0, 
    .partition_options         = PAR_OPT_READ_DIS | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_OTA_TEMP] =  
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "OTAStorage",
    .partition_start_addr      = 0,
    .partition_length          = 0, //204k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [HAL_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0xFA000,//0xFA000,
    .partition_length          = 0x1000, // 4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [HAL_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "PARAMETER2",
    .partition_start_addr      = 0xFB000,//0xFB000,
    .partition_length          = 0x2000, //8k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
    [HAL_PARTITION_PARAMETER_3] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "PARAMETER3",
    .partition_start_addr      = 0xFD000,//0xFD000,
    .partition_length          = 0x1000, //4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
	[HAL_PARTITION_PARAMETER_4] =
  {
    .partition_owner           = HAL_FLASH_EMBEDDED,
    .partition_description     = "PARAMETER4",
    .partition_start_addr      = 0xFE000,//0xFE000,
    .partition_length          = 0x1000, //4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  }
};