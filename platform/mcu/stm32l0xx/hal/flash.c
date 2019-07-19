/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/hal/flash.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal_flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];
static uint8_t mempool[FLASH_PAGE_SIZE];

static int32_t _flash_unlock_erase(uint32_t address, uint32_t len_bytes)
{
    uint32_t PAGEError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    if ((address >= (FLASH_BASE + FLASH_SIZE)) || (address < FLASH_BASE)) {
        return -1;
    }

    HAL_FLASH_Unlock();

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = address;
    EraseInitStruct.NbPages     = len_bytes / FLASH_PAGE_SIZE;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        return -1;
    }
    return 0;
}

static int32_t _flash_read_at(uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    uint32_t *dst = pData;

    for (i = 0; i < len_bytes; i += 4) 
    {
        *(dst + i/4) = *(__IO uint32_t *)(address + i);
    }

    return 0;
}

static int32_t _flash_write_at(uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;

    for (i = 0; i < len_bytes; i += 4)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, *(pData + i/4)) != HAL_OK)
            return -1;
    }

    return 0;
}

static int32_t _flash_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *)data;

    do {
        uint32_t fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        int fl_offset = dst_addr - fl_addr;
        int len = MIN(FLASH_PAGE_SIZE - fl_offset, size);

        memset(mempool, 0, FLASH_PAGE_SIZE);
        _flash_read_at(fl_addr, (uint32_t *)mempool, FLASH_PAGE_SIZE);
        memcpy(mempool + fl_offset, src_addr, len);

        ret = _flash_unlock_erase(fl_addr, FLASH_PAGE_SIZE);
        if (ret != 0) {
            goto err;
        } else {
            ret = _flash_write_at(fl_addr, (uint32_t *)mempool, FLASH_PAGE_SIZE);
            if (ret != 0) {
                goto err;
            } else {
                dst_addr += len;
                src_addr += len;
                remaining -= len;
            }
        }
    } while ((ret == 0) && (remaining > 0));

err:
    return ret;
}

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(pno, p_partition_info);
    start_addr = p_partition_info->partition_start_addr + *poff;
    _flash_update(start_addr, buf, buf_size);

    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(pno, p_partition_info);

    if (poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length)
        return -1;

    start_addr = p_partition_info->partition_start_addr + *poff;
    _flash_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    int ret;
    int remaining = size;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(pno, p_partition_info);
    if (size + off_set > p_partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);
    ret = _flash_unlock_erase(start_addr, size);
    memset(mempool, 0xff, FLASH_PAGE_SIZE);
    while (ret == 0 && remaining > 0) {
        ret = _flash_write_at(start_addr, (uint32_t *)mempool, FLASH_PAGE_SIZE);
        if (ret != 0) {
            return ret;
        } else {
            start_addr += FLASH_PAGE_SIZE;
            remaining -= FLASH_PAGE_SIZE;
        }
    }
    return ret;
}

int32_t hal_flash_enable_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}
