/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "fatfs_ext/inc/ff_driver.h"
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NODISK;

    if (disk.nbr <= 0 || pdrv < 0 || pdrv >= disk.nbr)
      return stat;
	
	stat = disk.drv[pdrv]->disk_status();

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	if (disk.nbr <= 0 || pdrv < 0 || pdrv >= disk.nbr)
      return stat;
	
	stat = disk.drv[pdrv]->disk_initialize();

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count 	/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;
	
	if (pdrv < 0 || pdrv >= disk.nbr || buff == (void*)0 || count <= 0)
		return RES_PARERR; // Return if the parameter is invalid

	if(disk.nbr <= 0)
		return RES_NOTRDY;
	
	res = disk.drv[pdrv]->disk_read(buff, sector, count);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{       
	DRESULT res = RES_PARERR;
	int index = 0;
	
	if (pdrv < 0 || pdrv >= disk.nbr || buff == (void*)0 || count <= 0)
		return RES_PARERR; // Return if the parameter is invalid

	if(disk.nbr <= 0)
		return RES_NOTRDY;

	res = disk.drv[pdrv]->disk_write(buff, sector, count);
	
	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_PARERR;
	
	if (pdrv < 0 || pdrv >= disk.nbr)
		return RES_PARERR; // Return if the parameter is invalid

	if(disk.nbr <= 0)
		return RES_NOTRDY;	
	
	res = disk.drv[pdrv]->disk_ioctl(cmd, buff);

	return res;
}
#endif

DWORD get_fattime (void)
{
	DWORD time_abs;
	
	time_abs = ((DWORD)(2016 - 1980) << 25) /* Fixed to Feb. 2, 2016 */
		| ((DWORD)2 << 21)
		| ((DWORD)2 << 16)
		| ((DWORD)0 << 11)
		| ((DWORD)0 << 5)
		| ((DWORD)0 >> 1);
	
	return time_abs;
}
