/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"
#include <aos/kernel.h>
#include "aos/kernel.h"

#include "esp_system.h"

/* Logic partition on flash devices for 4M bytes 1024*1024 */
const hal_logic_partition_t hal_partitions_4M_1024x1024[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0000,
	    .partition_length           = 0x1000,    //4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x310000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x311000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x1000,
	    .partition_length           = 0x100000, //1MB bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x101000,
        .partition_length          = 0x100000, //1MB bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x313000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x314000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

/* Logic partition on flash devices for 2M bytes 1024*1024 */
const hal_logic_partition_t hal_partitions_2M_1024x1024[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0000,
	    .partition_length           = 0x1000,    //4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x1f6000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x1f7000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x1000,
	    .partition_length           = 0xF4000, //976 bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x101000,
        .partition_length          = 0xF4000, //976 bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x1F9000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x1FA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

/* Logic partition on flash devices for 1M bytes 512*512 */
const hal_logic_partition_t hal_partitions_1M_512x512[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0000,
	    .partition_length           = 0x1000,    //4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0xF6000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0xF7000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x1000,
	    .partition_length           = 0x75000, //468 bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x81000,
        .partition_length          = 0x75000, //468 bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0xF9000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0xFA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

/* Logic partition on flash devices for 2M bytes 512*512 */
const hal_logic_partition_t hal_partitions_2M_512x512[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0000,
	    .partition_length           = 0x1000,    //4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x1F6000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x1F7000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x1000,
	    .partition_length           = 0x7B000, //492 bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x81000,
        .partition_length          = 0x7B000, //492 bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x1F9000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x1FA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

/* Logic partition on flash devices for 4M bytes 512*512 */
const hal_logic_partition_t hal_partitions_4M_512x512[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0000,
	    .partition_length           = 0x1000,    //4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x3F6000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x3F7000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x1000,
	    .partition_length           = 0x7B000, //492 bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x81000,
        .partition_length          = 0x7B000, //492 bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x3F9000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x3FA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

uint32_t user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32_t rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void flash_partition_init(void)
{

}

