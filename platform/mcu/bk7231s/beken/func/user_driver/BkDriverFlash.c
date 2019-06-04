/**
 ******************************************************************************
 * @file    BkDriverFlash.h
 * @brief   This file provides all the headers of Flash operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */
#include "include.h"
#include "rtos_pub.h"
#include "BkDriverFlash.h"
#include "flash_pub.h"
#include "drv_model_pub.h"
#include "error.h"
#include "uart_pub.h"
#include "mem_pub.h"
/* Logic partition on flash devices */
const bk_logic_partition_t bk7231_partitions[BK_PARTITION_MAX] =
{
    [BK_PARTITION_BOOTLOADER] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Bootloader",
        .partition_start_addr      = 0x00000000,
        .partition_length          = 0x1b000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_APPLICATION] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Application",
        .partition_start_addr      = 0x22000,
        .partition_length          = 0x1be000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "RF Firmware",
        .partition_start_addr      = 0x1e0000,// for rf related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_NET_PARAM] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "NET info",
        .partition_start_addr      = 0x1e1000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    }
};

static void BkFlashPartitionAssert( bk_partition_t inPartition )
{
    ASSERT(BK_PARTITION_BOOTLOADER < BK_PARTITION_MAX);
}

static uint32_t BkFlashPartitionIsValid( bk_partition_t inPartition )
{
    if((inPartition >= BK_PARTITION_BOOTLOADER)&&(inPartition < BK_PARTITION_MAX))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bk_logic_partition_t *bk_flash_get_info( bk_partition_t inPartition )
{
    bk_logic_partition_t *pt = NULL;

    BkFlashPartitionAssert(inPartition);

    if(BkFlashPartitionIsValid(inPartition))
    {
        pt = (bk_logic_partition_t *)&bk7231_partitions[inPartition];
    }
	else
		pt =	NULL;
    return pt;
}

OSStatus BkFlashInit(void)
{
    return 0;
}

OSStatus BkFlashUninit(void)
{
    return 0;
}

OSStatus bk_flash_erase(bk_partition_t inPartition, uint32_t off_set, uint32_t size)
{
    uint32_t i;
    uint32_t param;
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t start_sector, end_sector;
    bk_logic_partition_t *partition_info;
    GLOBAL_INT_DECLARATION();

    partition_info = bk_flash_get_info(inPartition);
    start_sector = off_set >> 12;
    end_sector = (off_set + size - 1) >> 12;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    for(i = start_sector; i <= end_sector; i ++)
    {
        param = partition_info->partition_start_addr + (i << 12);
        GLOBAL_INT_DISABLE();
        ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&param);
        GLOBAL_INT_RESTORE();
    }

    return kNoErr;
}

OSStatus bk_flash_write( bk_partition_t inPartition, volatile uint32_t off_set, uint8_t *inBuffer , uint32_t inBufferLength)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t start_addr;
    bk_logic_partition_t *partition_info;
    GLOBAL_INT_DECLARATION();

    ASSERT(inBuffer);

    partition_info = bk_flash_get_info(inPartition);
    start_addr = partition_info->partition_start_addr + off_set;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);

    GLOBAL_INT_DISABLE();
    ddev_write(flash_hdl, inBuffer, inBufferLength, start_addr);
    GLOBAL_INT_RESTORE();

    return kNoErr;
}

OSStatus bk_flash_read( bk_partition_t inPartition, volatile uint32_t off_set, uint8_t *outBuffer, uint32_t inBufferLength)
{
    UINT32 status;
    uint32_t start_addr;
    DD_HANDLE flash_hdl;
    bk_logic_partition_t *partition_info;
    GLOBAL_INT_DECLARATION();

    ASSERT(outBuffer);

    partition_info = bk_flash_get_info(inPartition);
    start_addr = partition_info->partition_start_addr + off_set;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);

    GLOBAL_INT_DISABLE();
    ddev_read(flash_hdl, outBuffer, inBufferLength, start_addr);
    GLOBAL_INT_RESTORE();

    return kNoErr;
}

OSStatus bk_flash_enable_security(PROTECT_TYPE type )
{
	DD_HANDLE flash_hdl;
    UINT32 status;
	uint32_t param = type;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&param);

    return kNoErr;
}


OSStatus test_flash_write(volatile uint32_t start_addr, uint32_t len)
{
 	UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t i;
	u8 buf[256];
	uint32_t addr = start_addr;
	uint32_t length = len;
	uint32_t tmp = addr + length;
	
	for(i=0;i<256;i++)
		buf[i]=i;
	
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
	for(;addr<tmp;addr+=256)
	{
		os_printf("write addr(size:256):%d\r\n",addr);
    	ddev_write(flash_hdl, buf, 256, addr);
	}
	
	return kNoErr;
}
OSStatus test_flash_erase(volatile uint32_t start_addr, uint32_t len)
{
 	UINT32 status;
    DD_HANDLE flash_hdl;
   
	uint32_t addr = start_addr;
	uint32_t length = len;
	 uint32_t tmp = addr+length;
	 
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
	for(;addr<tmp;addr+=0x1000)
	{
		os_printf("erase addr:%d\r\n",addr);
    	ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&addr);
		
	}
	return kNoErr;
}

OSStatus test_flash_read(volatile uint32_t start_addr, uint32_t len)
{
 	UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t i,j,tmp;
	u8 buf[256];
	uint32_t addr = start_addr;
	uint32_t length = len;
	tmp = addr+length;
	
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
	for(;addr<tmp;addr+=256)
	{
		os_memset(buf,0,256);
    	ddev_read(flash_hdl, buf, 256, addr);
		os_printf("read addr:%x\r\n",addr);
		for(i=0;i<16;i++)
		{
			for(j=0;j<16;j++)
			{
				os_printf("%02x ",buf[i*16+j]);
			}
			os_printf("\r\n");
		}
	}
	
	return kNoErr;
}

OSStatus test_flash_read_time(volatile uint32_t start_addr, uint32_t len)
{
 	UINT32 status, time_start, time_end;
    DD_HANDLE flash_hdl;
    uint32_t i,j,tmp;
	u8 buf[256];
	uint32_t addr = start_addr;
	uint32_t length = len;
	tmp = addr+length;
	
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    beken_time_get_time((beken_time_t *)&time_start);
    os_printf("read time start:%d\r\n", time_start);
	for(;addr<tmp;addr+=256)
	{
		os_memset(buf,0,256);
    	ddev_read(flash_hdl, buf, 256, addr);
	}
    beken_time_get_time((beken_time_t *)&time_end);
    os_printf("read time end:%d\r\n", time_end);

    os_printf("cost time:%d\r\n", time_end - time_start);
	
	return kNoErr;
}

// EOF

