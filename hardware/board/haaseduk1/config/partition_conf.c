/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

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
                (void)aos_flashpart_unregister(j);
            return ret;
        }
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(flash_partitions_init)
