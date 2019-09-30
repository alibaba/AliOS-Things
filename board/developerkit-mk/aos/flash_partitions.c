/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>

#include "aos/hal/flash.h"

const hal_logic_partition_t hal_partitions[] =
{
#ifdef JSENGINE
    /* JSENGINE Needs more flash, so ota was removed by now */
    [HAL_PARTITION_BOOTLOADER] =
    {
    .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "Bootloader",
    .partition_start_addr       = 0x08000000,
    .partition_length           = 0x1800,    /* 6k bytes */
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [HAL_PARTITION_2ND_BOOT] =
    {
    .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "OTA 2nd boot",/* 2nd boot */
    .partition_start_addr       = 0x08001800,
    .partition_length           = 0x7800,  /* 30k bytes */
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_APPLICATION] =
    {
    .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "Application",
    .partition_start_addr       = 0x08009000,
    .partition_length           = 0x87000, /* 540k bytes */
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x08090000,
        .partition_length           = 0x1000, /* 4k bytes */
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x08091000,
        .partition_length           = 0x2000, /* 8k bytes */
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x08093000,
        .partition_length           = 0x1000, /* 4k bytes */
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x08094000,
        .partition_length           = 0x1000, /* 4k bytes */
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_SPIFFS] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "spiffs",
        .partition_start_addr       = 0x080C0000,
        .partition_length           = 0x40000, /* 256k bytes */
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#else
    [HAL_PARTITION_BOOTLOADER] =
    {
	.partition_owner            = HAL_FLASH_EMBEDDED,
	.partition_description      = "Bootloader",
	.partition_start_addr       = 0x08000000,
	.partition_length           = 0x1800,    //6k bytes
	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [HAL_PARTITION_2ND_BOOT] =
    {
	.partition_owner            = HAL_FLASH_EMBEDDED,
	.partition_description      = "OTA 2nd boot",//2nd boot
	.partition_start_addr       = 0x08001800,
	.partition_length           = 0x7800,  //30k bytes
	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_APPLICATION] =
    {
	.partition_owner            = HAL_FLASH_EMBEDDED,
	.partition_description      = "Application",
	.partition_start_addr       = 0x08009000,  //bank1 begain addr 0x8000000 ~ 0x8080000 512k
	.partition_length           = 0x74000, //464K bytes
	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x08080000, //bank2 begain addr 0x8080000 ~ 0x8100000 512k ;0x8080000 ~ 0x8089000 is the boot + 2nd boot
        .partition_length          = 0x20000, //128K bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x0807E000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x080FD000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x0807F000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x080FF000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
};

const size_t hal_partitions_amount = (sizeof(hal_partitions)/sizeof(hal_logic_partition_t));

