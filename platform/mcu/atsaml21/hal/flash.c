/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "aos/hal/flash.h"
#include "driver_init.h"


#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define ROUND_UP(a,b) (((a) / (b)) * (b) + (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(pno);
    start_addr = partition_info->partition_start_addr + *poff;

    flash_write(&FLASH_0, start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(pno);

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *poff;
    flash_read(&FLASH_0, start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr, end_addr;
    hal_logic_partition_t *partition_info;
    int ret;
    int remaining = size;

    uint32_t page_size = flash_get_page_size(&FLASH_0);

    partition_info = hal_flash_get_info(pno);
    if (size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), page_size);
    end_addr = ROUND_UP((partition_info->partition_start_addr + off_set + size), page_size);

    flash_erase(&FLASH_0, start_addr, (end_addr-start_addr)/page_size);

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}
