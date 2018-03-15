/*
 *  Routines to associate usb mass storage driver with FatFs
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "integer.h"
#include <disk_if/inc/usbdisk.h>

#if FATFS_DISK_USB
#include "us_intf.h"

DSTATUS USB_disk_status(void){
	MSC_RESULT res;
	if(USB_STORAGE_READY){
		res = us_getStatus();
		if(res == MSC_W_PROTECT)
			return STA_PROTECT;
		else
			return 0; // every thing is ok
	}
	return STA_NOINIT;
}

DSTATUS USB_disk_initialize(void){
	MSC_RESULT res;

	if(USB_STORAGE_READY)
		return 0;
	
	res = us_init();

	if(res == MSC_OK)
		return 0;
	else 
		return STA_NOINIT;
}

/* Read sector(s) --------------------------------------------*/
DRESULT USB_disk_read(BYTE *buff, DWORD sector, UINT count){
	if(!USB_STORAGE_READY)
		return RES_NOTRDY;
	
	if(us_read_blocks(buff, sector, count) == MSC_OK)
		return RES_OK;
	else
		return RES_ERROR;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT USB_disk_write(const BYTE *buff, DWORD sector, UINT count){
	if(!USB_STORAGE_READY)
		return RES_NOTRDY;
	
	if(us_write_blocks(buff, sector, count) == MSC_OK)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif

/* Write sector(s) --------------------------------------------*/
#if _USE_IOCTL == 1
DRESULT USB_disk_ioctl (BYTE cmd, void* buff){
	DRESULT res = RES_ERROR;
	DWORD last_blk_addr, block_size;

	if(!USB_STORAGE_READY)
		return RES_NOTRDY;
	
	switch(cmd){
		/* Generic command (used by FatFs) */

		case CTRL_SYNC:		/* Flush disk cache (for write functions) */
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
			if(us_getcap(&last_blk_addr, &block_size) == MSC_OK){
				*(DWORD*)buff = last_blk_addr + 1;
				res = RES_OK;
			}else
				res = RES_ERROR;
			break;
		case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
			if(us_getcap(&last_blk_addr, &block_size) == MSC_OK){
				*(DWORD*)buff = block_size;
				res = RES_OK;
			}else
				res = RES_ERROR;
			break;
		case GET_BLOCK_SIZE:	/* Get erase block size (for only f_mkfs()) */
			*(DWORD*)buff = 512;
			res = RES_OK;
			break;
		case CTRL_ERASE_SECTOR:/* Force erased a block of sectors (for only _USE_ERASE) */
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
			break;
	}
	return res;
}
#endif

ll_diskio_drv USB_disk_Driver ={
	.disk_initialize = USB_disk_initialize,
	.disk_status = USB_disk_status,
	.disk_read = USB_disk_read,
#if _USE_WRITE == 1
	.disk_write = USB_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = USB_disk_ioctl,
#endif
	.TAG = "USB"
};

#endif
