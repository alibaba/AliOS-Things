/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

//#include "platform_peripheral.h"
#include "aos/hal/flash.h"
#include "board.h"
#include "wm_osal.h"
#include "wm_internal_flash.h"

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))


extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];

    return logic_partition;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( in_partition );
    
    if(size + off_set > partition_info->partition_length){
        printf("\r\n hal_flash_erase err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, partition_info->partition_length, off_set, size);        
        return -1;
      }

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SEC_SIZE);

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
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( in_partition );
    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > partition_info->partition_length){
        printf("\r\n hal_flash_write err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, partition_info->partition_length, off_set, in_buf_len);        
        return -1;
    }
    start_addr = partition_info->partition_start_addr + *off_set;
    ret = tls_fls_write(start_addr, in_buf, in_buf_len);

    *off_set += in_buf_len;
    return ret;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( in_partition );
    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > partition_info->partition_length){
        return -1;
        printf("\r\n hal_flash_read err, partition %d, part_len 0x%x, offset 0x%x, size 0x%x\r\n", in_partition, partition_info->partition_length, off_set, out_buf_len);        
    }
    
    start_addr = partition_info->partition_start_addr + *off_set;
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

