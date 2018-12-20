/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void flash_partition_init(void)
{
    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x08000000;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = 0x1B000; //108k bytes
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    /* used by ota, if didn't support ota , we can ignore this partition*/
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x0801B000;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = 0x1000; // 4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    /* used by kv, if didn't support kv , we can ignore this partition*/
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x0801C000;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = 0x2000; //8k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    /*It's useless for now*/
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description      = "PARAMETER3";
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr       = 0x0801E000;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    /* used by security, if didn't support security , we can ignore this partition*/
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER4";
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0x0801F000;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
}

