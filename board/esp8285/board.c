/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"
#include <aos/kernel.h>
#include "esp_system.h"

/* Logic partition on flash devices for 1M bytes 512*512 */
const hal_logic_partition_t hal_partitions_1M_512x512[] =
{
    [HAL_PARTITION_BOOTLOADER] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Bootloader",
        .partition_start_addr       = 0x0000,
        .partition_length           = 0x6000,    //24k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = 0x6000,
        .partition_length           = 0xA4000, //656k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0xAA000,
        .partition_length          = 0x46000, //280k bytes remaining 20k for diff ota
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
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

uint32_t user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32_t rf_cal_sec = 0;
    if(size_map == FLASH_SIZE_8M_MAP_512_512) {
        rf_cal_sec = 256 - 5;
    }
    return rf_cal_sec;
}
