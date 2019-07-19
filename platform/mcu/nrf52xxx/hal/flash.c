/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "k_api.h"

#include "aos/hal/flash.h"

#include "nrf.h"
#include "nrf_soc.h"
#include "nordic_common.h"
#include "nrf_fstorage.h"
#include "nrf_drv_clock.h"
#include "nrf_fstorage_nvmc.h"

#define FLASH_ERASE_PAGE_SIZE     (0x1000)

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FLASH_ALIGN_MASK ~(sizeof(uint32_t) - 1)
#define FLASH_ALIGN sizeof(uint32_t)

extern const hal_logic_partition_t hal_partitions[];
static nrf_fstorage_t g_fstorage;
static int g_fstorage_init = 0;

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

static nrf_fstorage_t* get_fstorage_instance(hal_logic_partition_t *partition_info)
{
    int ret = 0;

    if (partition_info == NULL)
        return NULL;

    g_fstorage.start_addr = partition_info->partition_start_addr;
    g_fstorage.end_addr = partition_info->partition_start_addr + partition_info->partition_length;

    if (g_fstorage_init == 0) {
        g_fstorage.evt_handler = NULL;
        ret = nrf_fstorage_init(&g_fstorage, &nrf_fstorage_nvmc, NULL);
        if (ret != 0)
            return NULL;

        g_fstorage_init++;
    }

    return &g_fstorage;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr, len, left_off;
    int32_t ret = 0;
    uint8_t *buffer = NULL;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nrf_fstorage_t *p_fs;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();

    hal_flash_info_get( pno, p_partition_info );
    p_fs = get_fstorage_instance(p_partition_info);
    if (p_fs == NULL)
        return -1;
    
    start_addr = p_partition_info->partition_start_addr + *poff;
    left_off = start_addr % FLASH_ALIGN;
    len = ((buf_size + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;

    if (len > buf_size || left_off > 0) {
        buffer = (uint8_t *)krhino_mm_alloc(len);
        if (!buffer)
            return -1;
        memset(buffer, 0xff, len);
        memcpy(buffer + left_off, buf, buf_size);

        RHINO_CRITICAL_ENTER();
        ret = nrf_fstorage_write(p_fs, start_addr - left_off, (uint32_t *)buffer, len, NULL);
        RHINO_CRITICAL_EXIT();

        krhino_mm_free(buffer);
    } else {
        RHINO_CRITICAL_ENTER();
        ret = nrf_fstorage_write(p_fs, start_addr, (uint32_t *)buf, len, NULL);
        RHINO_CRITICAL_EXIT();
    }

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int32_t ret = 0;
    uint32_t start_addr, len, left_off;
    uint8_t *buffer = NULL;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nrf_fstorage_t *p_fs;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();

    hal_flash_info_get( pno, p_partition_info );
    p_fs = get_fstorage_instance(p_partition_info);
    if (p_fs == NULL)
        return -1;

    if(poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length)
        return -1;
    start_addr = p_partition_info->partition_start_addr + *poff;

    left_off = start_addr % FLASH_ALIGN;
    len = ((buf_size + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;

    if (len > buf_size || left_off > 0) {
        buffer = (uint8_t *)krhino_mm_alloc(len);
        if (!buffer)
            return -1;
        memset(buffer, 0, len);
        ret = nrf_fstorage_read(p_fs, start_addr - left_off, (uint32_t *)buffer, len);

        memcpy(buf, buffer + left_off, buf_size);
        krhino_mm_free(buffer);
    } else
        ret = nrf_fstorage_read(p_fs, start_addr, buf, buf_size);

    *poff += buf_size;

    return ret;

}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t addr, page_size;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nrf_fstorage_t *p_fs;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();

    ret = hal_flash_info_get( pno, p_partition_info );
    if(ret != 0 || size + off_set > p_partition_info->partition_length)
        return -1;

    p_fs = get_fstorage_instance(p_partition_info);
    if (p_fs == NULL)
        return -1;

    if (p_fs->p_flash_info != NULL)
        page_size = p_fs->p_flash_info->erase_unit;
    else
        return -1;

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), page_size);
    end_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set + size - 1), page_size);

    for (addr = start_addr; addr <= end_addr; addr += page_size) {
        RHINO_CRITICAL_ENTER();
        ret = nrf_fstorage_erase(p_fs, addr, 1, NULL);
        RHINO_CRITICAL_EXIT();
        if (ret != 0)
            return ret;
    }

    return 0;

}


uint32_t hal_flash_erase_sector_size()
{
    return FLASH_ERASE_PAGE_SIZE;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


