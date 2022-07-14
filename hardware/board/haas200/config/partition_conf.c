/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

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
