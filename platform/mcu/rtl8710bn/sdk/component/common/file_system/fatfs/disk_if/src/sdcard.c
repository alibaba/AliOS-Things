/*
 *  Routines to associate SD card driver with FatFs
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "integer.h"
#include <disk_if/inc/sdcard.h>

#if	FATFS_DISK_SD

#include "sd.h" // sd card driver with sdio interface

#define SD_BLOCK_SIZE	512

static int interpret_sd_result(SD_RESULT result){
	int ret = 0;
	if(result == SD_OK)
		ret = 0;
	else if(result == SD_NODISK)
		ret = STA_NODISK;
	else if(result == SD_INITERR)
		ret = STA_NOINIT;
	else if(result == SD_PROTECTED)
		ret = STA_PROTECT;
	return ret;
}

DSTATUS SD_disk_status(void){
	SD_RESULT res;
	res = SD_Status();
	return interpret_sd_result(res);;
}

DSTATUS SD_disk_initialize(void){
	SD_RESULT res;
	res = SD_Init();
	return interpret_sd_result(res);
}

/* Read sector(s) --------------------------------------------*/
DRESULT SD_disk_read(BYTE *buff, DWORD sector, UINT count){
	SD_RESULT res;

	res = SD_ReadBlocks(sector, buff, count);

	//__rtl_memDump_v1_00(buff, 512*count, "MMC_disk_read:");
	
	return interpret_sd_result(res);
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT SD_disk_write(const BYTE *buff, DWORD sector, UINT count){
	SD_RESULT res;

	res = SD_WriteBlocks(sector, buff, count);
	
	return interpret_sd_result(res);
}
#endif

/* Write sector(s) --------------------------------------------*/
#if _USE_IOCTL == 1
DRESULT SD_disk_ioctl (BYTE cmd, void* buff){
	DRESULT res = RES_ERROR;
	SD_RESULT result;
	DWORD last_blk_addr, block_size;

	switch(cmd){
		/* Generic command (used by FatFs) */
		
		/* Make sure that no pending write process in the physical drive */
		case CTRL_SYNC:		/* Flush disk cache (for write functions) */
			result = SD_WaitReady();
			res = interpret_sd_result(result);
			break;
		case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
			result = SD_GetCapacity((unsigned long*) buff);
			res = interpret_sd_result(result);
			break;
		/* for case _MAX_SS != _MIN_SS */
		case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
			//*(DWORD*)buff = 1024;//2048;//4096;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:	/* Get erase block size (for only f_mkfs()) */
			*(DWORD*)buff = SD_BLOCK_SIZE;
			res = RES_OK;
			break;
		case CTRL_ERASE_SECTOR:/* Force erased a block of sectors (for only _USE_ERASE) */
			res = RES_OK;
			break;

		/* MMC/SDC specific ioctl command */

		case MMC_GET_TYPE:	/* Get card type */
			res = RES_OK;
			break;
		case MMC_GET_CSD:	/* Get CSD */
			res = RES_OK;
			break;
		case MMC_GET_CID:	/* Get CID */
			res = RES_OK;
			break;
		case MMC_GET_OCR:	/* Get OCR */
			res = RES_OK;
			break;
		case MMC_GET_SDSTAT:/* Get SD status */
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
			break;
	}
	return res;
}
#endif

ll_diskio_drv SD_disk_Driver ={
	.disk_initialize = SD_disk_initialize,
	.disk_status = SD_disk_status,
	.disk_read = SD_disk_read,
#if _USE_WRITE == 1
	.disk_write = SD_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = SD_disk_ioctl,
#endif
	.TAG	= "SD"
};
#endif
