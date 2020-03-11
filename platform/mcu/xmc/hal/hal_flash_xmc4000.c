/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "aos/hal/flash.h"
#include "hal_flash_xmc4000.h"
#include "xmc_flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

const uint32_t SECTOR_ADDR_TAB[17]={0x08000000,		// Sector 0,  16 Kbytes
	                                0x08004000,		// Sector 1,  16 Kbytes
	                                0x08008000,		// Sector 2,  16 Kbytes
	                                0x0800C000,		// Sector 3,  16 Kbytes
	                                0x08010000,		// Sector 4,  16 Kbytes
	                                0x08014000,		// Sector 5,  16 Kbytes
	                                0x08018000,		// Sector 6,  16 Kbytes
	                                0x0801C000,		// Sector 7,  16 Kbytes
	                                0x08020000,		// Sector 8, 128 Kbytes
	                                0x08040000,		// Sector 9, 256 Kbytes
	                                0x08080000,		// Sector 10,256 Kbytes
	                                0x080C0000,		// Sector 11,256 Kbytes
	                                0x08100000,		// Sector 12,256 Kbytes
	                                0x08140000,		// Sector 13,256 Kbytes
	                                0x08180000,		// Sector 14,256 Kbytes
	                                0x081C0000,		// Sector 15,256 Kbytes
	                                0x08200000};

extern const hal_logic_partition_t hal_partitions[];

static uint32_t GetSector(uint32_t addr);

int FLASH_write_at(uint32_t address, uint64_t *pData, uint32_t len_bytes)
{
    int i;
    int ret = -1;

    if((address&0xFF)!=0)
    {
        printf("Write address is incorrect @0x%08lx", address);
    	return 0;
    }

    for (i = 0; i < len_bytes; i += 256)
    {
        XMC_FLASH_ProgramPage((uint32_t*)(address + 0x04000000 + i), pData+(i/8));
    }
    /* Memory check */
    for (i = 0; i < len_bytes; i += 4)
    {
        uint32_t *dst = (uint32_t *)(address + i);
        uint32_t *src = ((uint32_t *) pData) + (i / 4);

        if ( *dst != *src )
        {
            printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) dst, *dst, *src);
            break;
        }
        ret = 0;
    }
    return ret;
}

int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes)
{
    int i;
    int ret = -1;
    uint32_t *src = (uint32_t *)(address+0x04000000);
    uint32_t *dst = (uint32_t *)(pData);

    for (i = 0; i < len_bytes; i += 4) {
        *(dst + i / 4) = *(src++);
    }

    ret = 0;
    return ret;
}

int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    uint32_t sector;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *) data;
    uint32_t fl_addr = 0;
    int fl_offset = 0;
    int len = 0;
    uint64_t *page_cache = NULL;

	sector=GetSector(dst_addr);
	if(sector<8)				//small sector
	{
    	page_cache = (uint64_t *)krhino_mm_alloc(XMC4000_S_SECTOR_SIZE);
    	if (page_cache == NULL) {
    	    return -1;
    	}
    	memset(page_cache, 0, XMC4000_S_SECTOR_SIZE);

    	do {
    	    fl_addr = ROUND_DOWN(dst_addr, XMC4000_S_SECTOR_SIZE);
    	    fl_offset = dst_addr - fl_addr;
    	    len = MIN(XMC4000_S_SECTOR_SIZE - fl_offset, remaining);

    	    /* Load from the flash into the cache */
    	    memcpy(page_cache, (void *) fl_addr, XMC4000_S_SECTOR_SIZE);
    	    /* Update the cache from the source */
    	    memcpy((uint8_t *)page_cache + fl_offset, src_addr, len);
    	    /* Erase the page, and write the cache */
    	    XMC_FLASH_EraseSector((uint32_t*)(fl_addr + 0x04000000));
    	    ret = FLASH_write_at(fl_addr, page_cache, XMC4000_S_SECTOR_SIZE);
    	    if (ret != 0) {
    	        printf("Error writing %lu bytes at 0x%08lx\n", XMC4000_S_SECTOR_SIZE, fl_addr);
    	    } else {
    	        dst_addr += len;
    	        src_addr += len;
    	        remaining -= len;
    	    }
    	} while ((ret == 0) && (remaining > 0));

    	krhino_mm_free(page_cache);
    	return ret;
    }

    else
    {
    	while(1)
    	{
    		if(remaining>XMC4000_PAGE_SIZE)
    		{
    		    ret = FLASH_write_at(dst_addr, src_addr, XMC4000_PAGE_SIZE);
    	        dst_addr += XMC4000_PAGE_SIZE;
    		    src_addr += XMC4000_PAGE_SIZE;
    		    remaining-= XMC4000_PAGE_SIZE;
    		}
    		else
    		{
    		    ret = FLASH_write_at(dst_addr, src_addr, remaining);
    	        dst_addr += remaining;
    		    src_addr += remaining;
    		    remaining = 0;
    		    return ret;
    		}
    	}
    }
}

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *partition_info;

    partition_info = (hal_logic_partition_t *)&hal_partitions[ pno ];
    memcpy(partition, partition_info, sizeof(hal_logic_partition_t));

    return 0;
}


hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf , uint32_t buf_size)
{
    int ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

    real_pno = pno;
    partition_info = hal_flash_get_info( real_pno );
    start_addr = partition_info->partition_start_addr + *poff;

    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length) {
        return -1;
    }
    start_addr = partition_info->partition_start_addr + *poff;

    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    int32_t ret=0;
    uint32_t start_addr;
    uint32_t sector;
    uint32_t sector_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;
    uint64_t *page_cache = NULL;

    real_pno = pno;
    partition_info = hal_flash_get_info( real_pno );
    if (size + off_set > partition_info->partition_length) {
        return -1;
    }

    page_cache = (uint64_t *)krhino_mm_alloc(XMC4000_S_SECTOR_SIZE);
    if (page_cache == NULL) {
        return -1;
    }
    memset(page_cache, 0xFF, XMC4000_S_SECTOR_SIZE);

	start_addr=partition_info->partition_start_addr+off_set;
	sector=GetSector(start_addr);

	while(1)
	{
		sector_addr=SECTOR_ADDR_TAB[sector];
		if(sector_addr>=start_addr+size)
		{
			break;
		}
    	XMC_FLASH_EraseSector((uint32_t*)(sector_addr+0x04000000));
    	if(sector<8)
    	{
    		if(FLASH_write_at(start_addr, page_cache, XMC4000_S_SECTOR_SIZE)!=0)
    		{
    			ret=-1;
    			break;
    		}
    	}
    	sector++;
    }

    krhino_mm_free(page_cache);
    return ret;
}

static uint32_t GetSector(uint32_t addr)
{
	uint32_t i;

	for(i=0; i<16; i++)
	{
		if((addr >= SECTOR_ADDR_TAB[i])&&(addr < SECTOR_ADDR_TAB[i+1]))
		{
			return i;
		}
	}
	return i;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    (void)partition;
    (void)off_set;
    (void)size;

    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    (void)partition;
    (void)off_set;
    (void)size;

    return 0;
}

