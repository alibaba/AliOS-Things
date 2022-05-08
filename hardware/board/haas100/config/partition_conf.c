/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <aos/mtdpart.h>
#include <aos/mtd.h>

/* Logic partition on flash devices */
const mtd_partition_t mtd_partitions[] =
{
    {
        .partition_name             = "boot1",
        .partition_name_std         = MTD_PART_NAME_BOOTLOADER,
        .partition_start_addr       = 0x0,
        .partition_length           = 0x10000,  //64KB
        .partition_options          = 0,
    },
    {
        .partition_name             = "boot_info",
        .partition_name_std         = MTD_PART_NAME_ENV2,
        .partition_start_addr       = 0x10000, //boot information need protect
        .partition_length           = 0x2000,  //8KB
        .partition_options          = 0,
    },
    {
        .partition_name             = "boot2A",
        .partition_name_std         = MTD_PART_NAME_2ND_BOOTLOADER,
        .partition_start_addr       = 0x12000,
        .partition_length           = 0x18000, //64KB
        .partition_options          = 0,
    },
    {
        .partition_name             = "RTOSA",
        .partition_name_std         = MTD_PART_NAME_KERNEL,
        .partition_start_addr       = 0x2A000,
        .partition_length           = 0x578000, //5.5MB
        .partition_options          = 0,
    },
    {
        .partition_name             = "boot2B",
        .partition_name_std         = MTD_PART_NAME_2ND_BOOTLOADER2,
        .partition_start_addr       = 0x5A2000,
        .partition_length           = 0x18000, //64KB
        .partition_options          = 0,
    },
    {
        .partition_name             = "RTOSB",
        .partition_name_std         = MTD_PART_NAME_KERNEL2,
        .partition_start_addr       = 0x5BA000,
        .partition_length           = 0x578000, //5.5MB
        .partition_options          = 0,
    },
    {
        .partition_name            = "littleFS",
        .partition_name_std        = MTD_PART_NAME_LITTLEFS,
        .partition_start_addr      = 0xB32000,
        .partition_length          = 0x4AE000, //4792KB
        .partition_options         = 0,
    },
    {
        .partition_name            = "boot1_sec",
        .partition_name_std        = MTD_PART_NAME_BOOTLOADER_SEC,
        .partition_start_addr      = 0xFE0000,
        .partition_length          = 0x10000, //64KB
        .partition_options         = 0,
    },
    {
        .partition_name            = "boot2_info",
        .partition_name_std        = MTD_PART_NAME_ENV,
        .partition_start_addr      = 0xFF0000,
        .partition_length          = 0x1000, //4KB
        .partition_options         = 0,
    },
    {
        .partition_name            = "KV",
        .partition_name_std        = MTD_PART_NAME_KV,
        .partition_start_addr      = 0xFF1000,
        .partition_length          = 0xD000, //52KB
        .partition_options         = 0,
    },
    {
        .partition_name            = "factory",
        .partition_name_std        = MTD_PART_NAME_FACTORY,
        .partition_start_addr      = 0xFFE000,
        .partition_length          = 0x2000,  //8KB
        .partition_options         = 0,
    }
};

/* Declare a constant to indicate the defined partitions amount */
const int mtd_partitions_amount = (sizeof(mtd_partitions) / sizeof(mtd_partition_t));

#include <aos/flashpart_core.h>
#include <aos/hal/flash.h>

static int flash_partitions_init(void)
{
    static aos_flashpart_t partitions[] = {
        {
            .dev = { .id = HAL_PARTITION_BOOTLOADER, },
            .flash_id = 0,
            .block_start = 0x0,
            .block_count = 0x10,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_3, },
            .flash_id = 0,
            .block_start = 0x10,
            .block_count = 0x2,
        },
        {
            .dev = { .id = HAL_PARTITION_BOOT1, },
            .flash_id = 0,
            .block_start = 0x12,
            .block_count = 0x18,
        },
        {
            .dev = { .id = HAL_PARTITION_APPLICATION, },
            .flash_id = 0,
            .block_start = 0x2A,
            .block_count = 0x578,
        },
        {
            .dev = { .id = HAL_PARTITION_BOOT1_REDUND, },
            .flash_id = 0,
            .block_start = 0x5A2,
            .block_count = 0x18,
        },
        {
            .dev = { .id = HAL_PARTITION_OTA_TEMP, },
            .flash_id = 0,
            .block_start = 0x5BA,
            .block_count = 0x578,
        },
        {
            .dev = { .id = HAL_PARTITION_LITTLEFS, },
            .flash_id = 0,
            .block_start = 0xB32,
            .block_count = 0x4AE,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_4, },
            .flash_id = 0,
            .block_start = 0xFE0,
            .block_count = 0x10,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_1, },
            .flash_id = 0,
            .block_start = 0xFF0,
            .block_count = 0x1,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_2, },
            .flash_id = 0,
            .block_start = 0xFF1,
            .block_count = 0xD,
        },
        {
            .dev = { .id = HAL_PARTITION_ENV, },
            .flash_id = 0,
            .block_start = 0xFFE,
            .block_count = 0x2,
        },
        {
            .dev = { .id = HAL_PARTITION_ENV_REDUND, },
            .flash_id = 0,
            .block_start = 0xFFE,
            .block_count = 0x2,
        },
    };

    for (size_t i = 0; i < sizeof(partitions) / sizeof(partitions[0]); i++) {
        int ret;

        ret = aos_flashpart_register(&partitions[i]);
        if (ret) {
            for (size_t j = 0; j < i; j++)
                (void)aos_flashpart_unregister(partitions[j].dev.id);
            return ret;
        }
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(flash_partitions_init)
