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
		.partition_description      = "BOOT0",
		.partition_start_addr       = 0x0,
		.partition_length           = 0x100000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_2ND_BOOT] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "BOOT1",
		.partition_start_addr       = 0x100000,
		.partition_length           = 0x100000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_ENV] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "KV",
		.partition_start_addr       = 0x43000000,
		.partition_length           = 0x1000000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_RTOSA] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "RTOSA",
		.partition_start_addr       = 0x7400000,
		.partition_length           = 0x6600000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_RTOSB] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "RTOSB",
		.partition_start_addr       = 0xda00000,
		.partition_length           = 0x6600000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_LITTLEFS] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "DATA",
		.partition_start_addr       = 0x40000000,
		.partition_length           = 0x3000000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_LITTLEFS2] =
	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "SYSTEM",
		.partition_start_addr       = 0x44000000,
		.partition_length           = 0x4000000,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
};

/* Declare a constant to indicate the defined partitions amount */
const size_t hal_partitions_amount = (sizeof(hal_partitions)/sizeof(hal_logic_partition_t));
