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
        .partition_name_std         = MTD_PART_NAME_ENV1,
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
