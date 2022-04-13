/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
#include "flash.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "hal_norflash.h"

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
    if(pno >= HAL_PARTITION_MAX)
    {
        return NULL;
    }
    return &hal_partitions[pno];
}

int flash_erase(unsigned int offset, unsigned int len)
{
    int                    ret = 0;
    unsigned int           addr;
    unsigned int           start_addr;
    unsigned int           end_addr;
    uint32_t lock = 0;

    start_addr = ROUND_DOWN(offset, SECTOR_SIZE);
    end_addr = ROUND_DOWN((offset + len - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        lock = int_lock();
        pmu_flash_write_config();
        hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
        ret = hal_norflash_erase(HAL_NORFLASH_ID_0, addr, SECTOR_SIZE);
        hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
        if (ret != HAL_NORFLASH_OK) {
            printf("error %s %d, ret:%d", __func__, __LINE__, ret);
        }
        pmu_flash_read_config();
        int_unlock(lock);
    }

    return 0;
}

int flash_read_data(unsigned int offset, unsigned char *buf, unsigned int len)
{
    int                     ret = 0;
    unsigned int           lock = 0;
    volatile char *flashPointer = NULL;

    if (buf == NULL) {
        return -1;
    }

    if (len == 0) {
        return -1;
    }

    lock = int_lock();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
	flashPointer = (volatile char *)(FLASH_NC_BASE + offset);
	memcpy(buf, (void *)flashPointer, len);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
	int_unlock(lock);

    if (ret != HAL_NORFLASH_OK) {
        printf("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
    }

    return 0;
}

int flash_write_data(unsigned int offset, unsigned char *buf, unsigned int len)
{
    int           ret = 0;
    unsigned int lock = 0;

    if (buf == NULL) {
        return -1;
    }

    if (len == 0) {
        return -1;
    }

    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    ret = hal_norflash_write(HAL_NORFLASH_ID_0, offset, buf, len);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    pmu_flash_read_config();
    int_unlock(lock);

    if (ret != HAL_NORFLASH_OK) {
        printf("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
    }

    return 0;
}