/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/mtdpart.h>
#include <aos/mtd.h>

/* Logic partition on flash devices */
const mtd_partition_t mtd_partitions[] =
{
    {
        .partition_name             = "BOOTLOADER",
        .partition_name_std         = MTD_PART_NAME_BOOTLOADER,
        .partition_start_addr       = 0x4000,
        .partition_length           = 0x2000,    /* 64KB */
        .partition_options          = 0,
    },
    {
        .partition_name             = "PARAMETE1",
        .partition_name_std         = MTD_PART_NAME_ENV,
        .partition_start_addr       = 0x3FF000,
        .partition_length           = 0x1000, /* boot env, 4KB */
        .partition_options          = 0,
    },
    {
        .partition_name             = "PARAMETER2",
        .partition_name_std         = MTD_PART_NAME_KV,
        .partition_start_addr       = 0x3FD000,
        .partition_length           = 0x2000, /* kv, 8KB */
        .partition_options          = 0,
    },
    {
        .partition_name             = "PARAMETER3",
        .partition_name_std         = MTD_PART_NAME_BOOTLOADER_SEC,
        .partition_start_addr       = 0x3FC000,
        .partition_length           = 0x1000, /* security,factory, 4KB */
        .partition_options          = 0,
    },
    {
        .partition_name             = "PARAMETER4",
        .partition_name_std         = MTD_PART_NAME_UNKNOWN,
        .partition_start_addr       = 0x3FB000,
        .partition_length           = 0x1000, /* reserved, 4KB*/
        .partition_options          = 0,
    },
    {
        .partition_name             = "APPLICATION",
        .partition_name_std         = MTD_PART_NAME_KERNEL,
        .partition_start_addr       = 0x14000,
        .partition_length           = 0x200000, /* OTA A system, 2MB */
        .partition_options          = 0,
    },
    {
        .partition_name            = "APPLICATION_B",
        .partition_name_std         = MTD_PART_NAME_KERNEL2,
        .partition_start_addr      = 0x214000,
        .partition_length          = 0x140000, /* OTA B system, 1.25MB */
        .partition_options         = 0,
    },
    {
        .partition_name            = "LITTLEFS",
        .partition_name_std         = MTD_PART_NAME_LITTLEFS,
        .partition_start_addr      = 0x354000,
        .partition_length          = 0xa7000, /* littlefs, 668KB*/
        .partition_options         = 0,
    },
    {
        .partition_name            = "2NDBOOT",
        .partition_name_std         = MTD_PART_NAME_2ND_BOOTLOADER,
        .partition_start_addr      = 0x6000,
        .partition_length          = 0xe000, /* 2nd bootloader, 56KB*/
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
            .block_start = 0x4,
            .block_count = 0x2,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_1, },
            .flash_id = 0,
            .block_start = 0x3FF,
            .block_count = 0x1,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_2, },
            .flash_id = 0,
            .block_start = 0x3FD,
            .block_count = 0x2,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_4, },
            .flash_id = 0,
            .block_start = 0x3FC,
            .block_count = 0x1,
        },
        {
            .dev = { .id = HAL_PARTITION_PARAMETER_3, },
            .flash_id = 0,
            .block_start = 0x3FB,
            .block_count = 0x1,
        },
        {
            .dev = { .id = HAL_PARTITION_APPLICATION, },
            .flash_id = 0,
            .block_start = 0x14,
            .block_count = 0x200,
        },
        {
            .dev = { .id = HAL_PARTITION_OTA_TEMP, },
            .flash_id = 0,
            .block_start = 0x214,
            .block_count = 0x140,
        },
        {
            .dev = { .id = HAL_PARTITION_LITTLEFS, },
            .flash_id = 0,
            .block_start = 0x354,
            .block_count = 0xA7,
        },
        {
            .dev = { .id = HAL_PARTITION_BOOT1, },
            .flash_id = 0,
            .block_start = 0x6,
            .block_count = 0xE,
        },
    };

    for (size_t i = 0; i < sizeof(partitions) / sizeof(partitions[0]); i++) {
        int ret;

        ret = aos_flashpart_register(&partitions[i]);
        if (ret) {
            for (size_t j = 0; j < i; j++)
                (void)aos_flashpart_unregister(j);
            return ret;
        }
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(flash_partitions_init)
