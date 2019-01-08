/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#include <k_api.h>
#include "aos/hal/flash.h"
#include "stm32f1xx_hal_flash.h"

#define FLASH_ALIGN_MASK ~(sizeof(uint16_t) - 1)
#define FLASH_ALIGN sizeof(uint16_t)
#define FLASH_SECTOR_SIZE  2048

#define ROUND_MASK(a)           ((a) - 1)
#define ROUND_UP(a)          (((a) + ROUND_MASK(FLASH_ALIGN)) & ~ROUND_MASK(FLASH_ALIGN))
#define ROUND_DOWN(a)        ((a) & ~ROUND_MASK(FLASH_ALIGN))

extern const hal_logic_partition_t hal_partitions[];


/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  sector = (Address - FLASH_BASE) / FLASH_SECTOR_SIZE;

  return sector;
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

static int32_t hal_flash_write_lowlevel(uint32_t start_addr, const void *buf, uint32_t buf_size)
{
    int ret = 0;
    uint8_t* src_data_p;
    uint32_t byte_count=0;//count the number of programmed bytes

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    src_data_p = (uint8_t*)buf;
    while(byte_count < buf_size)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, start_addr + byte_count, *(src_data_p++))!=HAL_OK)
        {
            ret =-1;
            break;
        }
        byte_count += FLASH_ALIGN;
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    return ret;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{
    int32_t ret=0;
    uint32_t start_addr;
    uint32_t len_l, len_r, len;
    uint8_t *buffer = NULL;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    start_addr = partition_info->partition_start_addr + *poff;
    if((*poff + buf_size) > partition_info->partition_length)
        return -1;

    len_l = ROUND_UP(start_addr) - start_addr;  // left unalign size
    len_r = (start_addr + buf_size) - ROUND_DOWN((start_addr + buf_size)); // right unalign size
    len = buf_size - len_l - len_r; // align size

    if(len != buf_size) {
        buffer = (uint8_t *)aos_malloc(FLASH_ALIGN);
        if (!buffer)
            return -1;
    }
    if (len_l != 0) {
        ret = FLASH_read_at(ROUND_DOWN(start_addr), buffer, FLASH_ALIGN);
        memcpy(buffer + FLASH_ALIGN - len_l, buf, len_l);
        ret = hal_flash_write_lowlevel(ROUND_DOWN(start_addr), (uint32_t *)buffer, FLASH_ALIGN);
    }

    if(len != 0) {
        ret = hal_flash_write_lowlevel(start_addr + len_l, (uint32_t *)((uint32_t)buf + len_l), len);
    }

    if (len_r != 0) {
        ret = FLASH_read_at(ROUND_DOWN(start_addr + buf_size), buffer, FLASH_ALIGN);
        memcpy(buffer, (char *)buf + len_l + len, len_r);
        ret = hal_flash_write_lowlevel(ROUND_DOWN(start_addr + buf_size), (uint32_t *)buffer, FLASH_ALIGN);
    }

    *poff += buf_size;
    if(len != buf_size) {
        aos_free(buffer);
    }

    return ret;
}


int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    int32_t ret=0;
    hal_logic_partition_t *partition_info;
    uint32_t start_addr;
    uint32_t i=0;

    partition_info = hal_flash_get_info( pno );
    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
    {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr,(uint8_t*)buf,buf_size);

    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    int32_t ret=0;
    hal_logic_partition_t *partition_info;
    uint32_t FirstSector = 0, NbOfSectors = 0;
    uint32_t SECTORError = 0;
    FLASH_EraseInitTypeDef EraseInit;

    partition_info = hal_flash_get_info( pno );
    if (size + off_set > partition_info->partition_length) {
        return -1;
    }

    FirstSector = GetSector(partition_info->partition_start_addr+off_set);
    NbOfSectors = GetSector(partition_info->partition_start_addr+off_set + size -1)-FirstSector+1;

    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = partition_info->partition_start_addr+off_set;
    EraseInit.NbPages = NbOfSectors;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    if(HAL_FLASHEx_Erase(&EraseInit, &SECTORError) != HAL_OK)
    {
        ret =-1;
    }

     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return ret;
}


/**
  * @brief  Read from FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed: Must be 8 byte aligned.
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_read_at(uint32_t address, uint8_t *pData, uint32_t len_bytes)
{
    uint32_t i;
    int ret = -1;
    uint8_t *src = (uint8_t *)(address);
    uint8_t *dst = (uint8_t *)(pData);

    for (i = 0; i < len_bytes; i++)
    {
        *(dst++) = *(src++);
    }

    ret = 0;
    return ret;
}

#endif
