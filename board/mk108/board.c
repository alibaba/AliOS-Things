/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] = 
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0,
	    .partition_length           = 0x10000,    //64k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x10000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x11000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x12000,
	    .partition_length           = 0x9E000 + 0x40000,   //632k + 256k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0xB0000 + 0x40000,
        .partition_length          = 0x50000 - 0x40000 - 0x1000, //320k - 256k - 4k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0xFF000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

void board_init(void)
{
}
