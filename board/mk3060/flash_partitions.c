/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>

#include "aos/hal/flash.h"

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
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x11000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_2ND_BOOT] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA 2nd boot",
        .partition_start_addr       = 0x13000,
        .partition_length           = 0x6000,  //24k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x19000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x1A000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_APPLICATION] =
    {
	.partition_owner            = HAL_FLASH_EMBEDDED,
	.partition_description      = "Application",
	.partition_start_addr       = 0x1C000,
	.partition_length           = 0xE5000, //916K bytes
	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x101000,
        .partition_length          = 0xE5000, //916K bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x1F1000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

const size_t hal_partitions_amount = (sizeof(hal_partitions)/sizeof(hal_logic_partition_t));

