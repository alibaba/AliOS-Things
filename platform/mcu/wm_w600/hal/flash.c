/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
//#include "platform_peripheral.h"
#include "aos/hal/flash.h"
#include "board.h"
#include "wm_osal.h"
#include "wm_internal_flash.h"

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))


extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );
    
    if(size + off_set > p_partition_info->partition_length){
        printf("\r\n hal_flash_erase err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, p_partition_info->partition_length, off_set, size);        
        return -1;
      }

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SEC_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SEC_SIZE) {
        ret = tls_fls_erase(addr/SPI_FLASH_SEC_SIZE);
        if(ret != TLS_FLS_STATUS_OK )
            return ret;
    }

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );
    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > p_partition_info->partition_length){
        printf("\r\n hal_flash_write err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, p_partition_info->partition_length, off_set, in_buf_len);        
        return -1;
    }
    start_addr = p_partition_info->partition_start_addr + *off_set;
    ret = tls_fls_write(start_addr, in_buf, in_buf_len);

    *off_set += in_buf_len;
    return ret;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );
    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > p_partition_info->partition_length){
        return -1;
        printf("\r\n hal_flash_read err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, p_partition_info->partition_length, off_set, out_buf_len);        
    }
    
    start_addr = p_partition_info->partition_start_addr + *off_set;
    tls_fls_read(start_addr, out_buf, out_buf_len);
    
    *off_set += out_buf_len;
    return ret;
}

int32_t hal_flash_enable_secure(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

