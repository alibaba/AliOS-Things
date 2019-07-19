/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/flash.h"

#include "gd32f30x.h"
#include "gd32f30x_fmc_ext.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FMC_CACHE_SIZE  0x2000

extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    ErrStatus reval;
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;
    fmc_unlock();
    reval = xxx_fmc_region_write(start_addr, (void*)buf, buf_size);
    fmc_lock();
    if (SUCCESS != reval) {
        printf("HAL FLASH update failed! \r\n");
        return -1;
    }

    *poff += buf_size;

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;

    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    xxx_fmc_region_read(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + off_set;
    fmc_unlock();
    xxx_fmc_region_erase(start_addr, size);
    fmc_lock();
    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


