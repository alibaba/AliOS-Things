/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"

#define SPI_FLASH_CAPACITY  0x200000UL //total capacity
#define OTA_2ND_SEQ_START   0x100000UL //according to image-xip-xz.cfg file

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
    [HAL_PARTITION_BOOTLOADER] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "Bootloader",
        .partition_start_addr     = 0x0,
        .partition_length         = 0x8000, //32k bytes
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "PARAMETER1",
        .partition_start_addr     = SPI_FLASH_CAPACITY - 4 * 0x1000,
        .partition_length         = 0x1000, // 4k bytes
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "PARAMETER2",
        .partition_start_addr     = SPI_FLASH_CAPACITY - 3 * 0x1000,
        .partition_length         = 0x2000, //8k bytes
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "Application",
        .partition_start_addr     = 0x8000,   // from 32k bytes
        .partition_length         = (SPI_FLASH_CAPACITY/2) - 0x8000 - 4 * 0x1000,
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "OTA Storage",
        .partition_start_addr     = OTA_2ND_SEQ_START + 0x8000,
        .partition_length         = (SPI_FLASH_CAPACITY/2) - 0x8000 - 4 * 0x1000,
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner          = HAL_FLASH_EMBEDDED,
        .partition_description    = "PARAMETER4",
        .partition_start_addr     = SPI_FLASH_CAPACITY - 1 * 0x1000,
        .partition_length         = 0x1000, //4k bytes
        .partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};
