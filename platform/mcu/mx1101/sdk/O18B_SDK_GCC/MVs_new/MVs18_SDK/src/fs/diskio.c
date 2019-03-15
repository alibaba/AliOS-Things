/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:diskio.c
//maintainer lilu
/////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "fs_config.h"
#include "diskio.h"
#include "fsinfo.h"

#include "host_stor.h"
#include "sd_card.h"


//
// Inidialize a Drive
//
DSTATUS disk_initialize(
    uint8_t drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS res = RES_ERROR;

	switch(drv)
	{
#ifdef	FUNC_CARD_EN
		case DEV_ID_SD:
			//res = MMC_disk_initialize();
			break;
#endif	//FUNC_CARD_EN

#ifdef	FUNC_USB_EN
		case DEV_ID_USB:
//		res = USB_disk_initialize(drv);
			break;
#endif	//FUNC_CARD_EN

		default:
			break;
	}
	return res;
}


//
// Return Disk Status
//
DSTATUS disk_status(
    uint8_t drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS res = RES_ERROR;

	switch(drv)
	{
#ifdef	FUNC_CARD_EN
		case DEV_ID_SD :
			//res = MMC_disk_status();
			break;
#endif	//FUNC_CARD_EN

#ifdef	FUNC_USB_EN
		case DEV_ID_USB :
//		res = USB_disk_status(drv);
			break;
#endif	//FUNC_USB_EN

		default:
			break;
	}
	return res;
}

//
//Read Sector(s)
//
DRESULT disk_read(
    uint8_t drv,		/* Physical drive nmuber (0..) */
    uint8_t* buff,		/* Data buffer to store read data */
    uint32_t sector,	/* Sector address (LBA) */
    uint8_t count		/* Number of sectors to read (1..255) */
)
{
	DRESULT res = RES_ERROR;
	switch(drv)
	{
#ifdef	FUNC_CARD_EN
		case DEV_ID_SD :
			if(!SdReadBlock(sector, buff, count))
			{
				res	= RES_OK;
			}
			break;
#endif	//FUNC_CARD_EN

#ifdef	FUNC_USB_EN
		case DEV_ID_USB :
			if(HostStorReadBlock(sector, buff, count))
			{
				res	= RES_OK;
			}

#endif	//FUNC_USB_EN

		default:
			break;
	}

	return res;
}


//
// Write Sector(s)
//
DRESULT disk_write(
    uint8_t drv,			/* Physical drive nmuber (0..) */
    const uint8_t* buff,	/* Data to be written */
    uint32_t sector,		/* Sector address (LBA) */
    uint8_t count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res = RES_ERROR;

	switch(drv)
	{
#ifdef	FUNC_CARD_EN
		case DEV_ID_SD :
			if(!SdWriteBlock(sector, buff, count))
			{
				res	= RES_OK;
			}
			break;
#endif	//FUNC_CARD_EN

#ifdef	FUNC_USB_EN
		case DEV_ID_USB :
			res = HostStorWriteBlock(sector, (void*)buff, count) ? RES_OK : RES_ERROR;
			break;
#endif	//FUNC_USB_EN

		default:
			break;
	}
	return res;
}

