#include "hal/soc/soc.h"
#include <hal_spi_flash.h>

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define SPI_FLASH_OFFSET(address) ((uint32_t)(address)&0x00ffffff)
#define SPI_FLASH_UNCACHE_ADDRESS(offset) ((void *)(SPI_FLASH_OFFSET(offset) | 0xa8000000))
#define SPI_FLASH_SECTOR_SIZE (4 * 1024)

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    start_addr = partition_info->partition_start_addr + *poff;
    ret = hal_SpiFlashWrite(start_addr, (void *)buf, buf_size);

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int32_t ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    start_addr = (uint32_t)SPI_FLASH_UNCACHE_ADDRESS(start_addr);
    memcpy(buf, (void *)start_addr, buf_size); 
    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), SPI_FLASH_SECTOR_SIZE);
    end_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SECTOR_SIZE) {
        ret = hal_SpiFlashErase(addr, SPI_FLASH_SECTOR_SIZE);
        if (ret != 0)
            return ret;
    }

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


