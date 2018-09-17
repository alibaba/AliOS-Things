/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "hal/soc/soc.h"
#include "c_types.h"
#include "spi_flash.h"
#include "esp_system.h"
#include "eagle_soc.h"
#include "rec_flash.h"
#include "upgrade.h"
#include "rec_pub.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FLASH_ALIGN_MASK ~(sizeof(uint32_t) - 1)
#define FLASH_ALIGN sizeof(uint32_t)

extern void vPortETSIntrLock(void);
extern void vPortETSIntrUnlock(void);

extern const hal_logic_partition_t hal_partitions_1M_512x512[];
extern const hal_logic_partition_t hal_partitions_2M_512x512[];
extern const hal_logic_partition_t hal_partitions_4M_512x512[];
extern const hal_logic_partition_t hal_partitions_2M_1024x1024[];
extern const hal_logic_partition_t hal_partitions_4M_1024x1024[];

static void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    uint32_t start_addr, size, left_off;
    uint8_t *buf = NULL;
    int ret;
 
    start_addr = address;
    left_off = start_addr % FLASH_ALIGN;
    
    size = ((len + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;
    
    if (size > len || left_off > 0) {
        buf = (uint8_t *)malloc(size);
        if (!buf)
            return;
        memset(buf, 0, size);
        ret = spi_flash_read(start_addr - left_off, (uint32_t *)buf, size);
        if(ret != 0)
        {
            printf("spi_flash_read fail 1, addr 0x%x, len 0x%x\n", start_addr, len);
        }  
        memcpy(buffer, buf + left_off, len);
        free(buf);
    } else {        
        ret = spi_flash_read(start_addr, (uint32_t *)buffer, len); 
        if(ret != 0)
        {
            printf("spi_flash_read fail 2, addr 0x%x, len 0x%x\n", start_addr, len);
        }       
    }
}

static void flash_write_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    uint32_t start_addr, size, left_off;
    uint8_t *buf = NULL;
    int ret;

    start_addr = address;

    left_off = start_addr % FLASH_ALIGN;
    size = ((len + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;
    
    if (size > len || left_off > 0) {
        buf = (uint8_t *)malloc(size);
        if (!buf)
            return;
        memset(buf, 0xff, size);
        memcpy(buf + left_off, buffer, len);
        vPortETSIntrLock();
        ret = spi_flash_write(start_addr - left_off, (uint32_t *)buf, len);
        vPortETSIntrUnlock();
        if(ret != 0)
        {
            printf("spi_flash_write fail 1, addr 0x%x, len 0x%x, ret %d\n", start_addr, len, ret);
        }
        free(buf);
    } else {
        vPortETSIntrLock();        
        ret = spi_flash_write(start_addr, (uint32_t *)buffer, len);        
        vPortETSIntrUnlock();
        if(ret != 0)
        {
            printf("spi_flash_write fail 2, addr 0x%x, len 0x%x, ret %d\n", start_addr, len, ret);
        }
    }
}

void flash_erase_sector(uint32_t offset,  uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;

    start_addr = ROUND_DOWN(offset, SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((offset + size - 1), SPI_FLASH_SEC_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SEC_SIZE) {
        vPortETSIntrLock();
        ret = spi_flash_erase_sector(addr/SPI_FLASH_SEC_SIZE);
        vPortETSIntrUnlock();
        if (ret != 0)
        {
            printf("flash_erase_sector fail , addr 0x%x\n", addr);
            return;
        }            
    }

    return;
}

/* address convert to offset in flash rom */
unsigned long rec_flash_addr2ofst(unsigned long addr)
{
    if ( addr % FLASH_WRITE_UNIT )
    {
        return (unsigned long)NULL;
    }
    return addr;
}

void rec_flash_init(void)
{    
    
}

/* offset means physical address */
void rec_flash_erase(unsigned long offset)
{
    offset &= ~(FLASH_SECTOR_SIZE-1);
    flash_erase_sector(offset, FLASH_SECTOR_SIZE);
}

/* offset means physical address */
void rec_flash_read_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{    
    flash_read_data((UINT8 *)buffer, offset, len);
}

/* offset means physical address */
void rec_flash_write_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    flash_write_data((UINT8 *)buffer, offset, len);
}

hal_logic_partition_t *rec_flash_get_info(hal_partition_t pno)
{
    return hal_flash_get_info(pno);
}

uint32_t rec_get_recflag_addr()
{
    uint32_t recflag_addr = 0;
    hal_logic_partition_t *logic_partition;

    logic_partition = rec_flash_get_info(HAL_PARTITION_PARAMETER_4);

    recflag_addr = logic_partition->partition_start_addr;

    return recflag_addr;  
}

