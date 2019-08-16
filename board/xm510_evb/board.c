/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <board.h>


/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x7E0000,
        .partition_length           = 0x20000,    //128k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
};

void aos_board_init(void)
{
}

