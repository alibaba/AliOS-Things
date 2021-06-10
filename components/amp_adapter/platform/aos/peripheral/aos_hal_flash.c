/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "aos_hal_flash.h"

int32_t aos_hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    return 0;
}

int32_t aos_hal_flash_read(hal_partition_t id, uint32_t *offset, void *buffer, uint32_t buffer_len)
{
    return 0;
}

int32_t aos_hal_flash_write(hal_partition_t id, uint32_t *offset, const void *buffer, uint32_t buffer_len)
{
    return 0;
}

int32_t aos_hal_flash_erase(hal_partition_t id, uint32_t offset, uint32_t size)
{
    return 0;
}
