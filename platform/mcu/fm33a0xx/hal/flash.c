/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "aos/hal/flash.h"
#include "string.h"
#include "fm33a0xx_rcc.h"
#include "fm33A0xx_flash.h"
#define FLASH_PAGE_SIZE   512
#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];
static uint32_t mempool[FLASH_PAGE_SIZE>>2];

static int32_t _flash_unlock_erase(hal_partition_t pno,uint32_t address, uint32_t len_bytes)
{
    int32_t i;
    int32_t Result = 0;
    uint32_t *PFlash;
    uint32_t PAGEError = 0;  
    hal_logic_partition_t *logic_partition;
    PFlash =(uint32_t *)address;
    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    if ((address >= (logic_partition->partition_start_addr + logic_partition->partition_length)) || (address < logic_partition->partition_start_addr)) {
        return -1;
    }
    
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE);
    for(i=0;i<(len_bytes+(FLASH_PAGE_SIZE-1))/FLASH_PAGE_SIZE;i++)
    {
      FLASH_Erase_Sector(address+(i*FLASH_PAGE_SIZE)); 
    }
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
   
    for( i=0;i<(len_bytes>>2);i++ )
    {
        if( PFlash[i] != 0xFFFFFFFF ) 
        {
            return -1;
        }
    }
    return 0;
}

static int32_t _flash_read_at(hal_partition_t pno,uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    uint32_t *dst = pData;
    memcpy(dst,(uint8_t*)address,len_bytes);
    return 0;
}

static int32_t _flash_write_at(hal_partition_t pno,uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    uint8_t *PFlash;
    uint8_t *pSrc;
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE);
    FLASH_Prog_ByteString(address,(uint8_t*)pData,len_bytes);
    PFlash = (uint8_t*)address;
    pSrc = (uint8_t*)pData;
    for( i=0;i<len_bytes;i++ )
    {
        if( PFlash[i] != pSrc[i] ) 
        {
            return -1;
        }
    }   
    return 0;
}

static int32_t _flash_update(hal_partition_t pno,uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *)data;

    do {
        uint32_t fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        int fl_offset = dst_addr - fl_addr;
        int len = MIN(FLASH_PAGE_SIZE - fl_offset, size);

        memset(mempool, 0, FLASH_PAGE_SIZE);
        _flash_read_at(pno,fl_addr, (uint32_t*)mempool, FLASH_PAGE_SIZE);
        memcpy(((uint8_t*)mempool) + fl_offset, src_addr, len);

        ret = _flash_unlock_erase(pno,fl_addr, FLASH_PAGE_SIZE);
        if (ret != 0) {
            goto err;
        } else {
            ret = _flash_write_at(pno,fl_addr, (uint32_t *)mempool, FLASH_PAGE_SIZE);
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

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;
    _flash_update(pno,start_addr, buf, buf_size);

    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *poff;
    _flash_read_at(pno,start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr;
    int ret;
    int remaining = size;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }


    if (size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);
    ret = _flash_unlock_erase(pno,start_addr, size);
    memset(mempool, 0xff, FLASH_PAGE_SIZE);
    while (ret == 0 && remaining > 0) {
        ret = _flash_write_at(pno,start_addr, (uint32_t *)mempool, FLASH_PAGE_SIZE);
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
