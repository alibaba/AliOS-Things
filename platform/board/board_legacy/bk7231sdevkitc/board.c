/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"
#include "aos/kernel.h"

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x000000,
        .partition_length           = 0x10000, //64k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_PARAMETER_1] =
	{
		.partition_owner			= HAL_FLASH_EMBEDDED,
		.partition_description		= "PARAMETER1",
		.partition_start_addr		= 0x010000,// boot information need protect
		.partition_length			= 0x1000, // 4k bytes
		.partition_options			= PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x011000,//cpu is 0x10000
        .partition_length           = 0xE1000, //900k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	/*reserved 52k*/
	[HAL_PARTITION_OTA_TEMP] =
	{
		.partition_owner		   = HAL_FLASH_EMBEDDED,
		.partition_description	   = "OTA Storage",
		.partition_start_addr	   = 0x0FF000,//cpu is 0xF0000
		.partition_length		   = 0xE1000, //900k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	/*protected flash size is 0~1984k. below are not protected.*/
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x1F0000,
        .partition_length           = 0x2000, // 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
		.partition_description		= "PARAMETER4",
		.partition_start_addr		= 0x1F2000,
		.partition_length			= 0x1000, // 4k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description		= "RF parameter",
		.partition_start_addr		= 0x1F3000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};


const size_t hal_partitions_amount = (sizeof(hal_partitions)/sizeof(hal_logic_partition_t));

void board_init(void)
{
}

int board_cli_init(void)
{
    return -1;
}

