/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "aos/hal/flash.h"

/* Logic partition on flash devices */
hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX];

void flash_partition_init(void)
{
    /*
     * First 512K reserved for mini boot loader. But only set the second
     * sector here, AOS should not touch the first sector.
     */
//    [HAL_PARTITION_BOOTLOADER].partition_owner            = HAL_FLASH_EMBEDDED,
//    [HAL_PARTITION_BOOTLOADER].partition_description      = "Mini Boot Loader",
//    [HAL_PARTITION_BOOTLOADER].partition_start_addr       = 0,
//    [HAL_PARTITION_BOOTLOADER].partition_length           = 0,
//    [HAL_PARTITION_BOOTLOADER].partition_options          = PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS,

    hal_logic_partition[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_logic_partition[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_logic_partition[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x00000000;
    hal_logic_partition[HAL_PARTITION_APPLICATION].partition_length           = 0x00040000;   // 256k bytes
    hal_logic_partition[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_logic_partition[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_logic_partition[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_logic_partition[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x00040000;
    hal_logic_partition[HAL_PARTITION_PARAMETER_2].partition_length           = 0x2000;//8k bytes
    hal_logic_partition[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS;

}

