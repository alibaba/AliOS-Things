/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
        
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x3F4000,
        .partition_length           = 0x2000, // 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    }

};



void board_init(void)
{
}
