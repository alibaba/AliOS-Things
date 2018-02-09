/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FF_DRIVER_H
#define __FF_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"
#include "ff.h"
#include "platform_opts.h"

typedef struct{
	DSTATUS (*disk_initialize) (void);                     /*!< Initialize Disk Drive                     */
	DSTATUS (*disk_status)     (void);                     /*!< Get Disk Status                           */
	DRESULT (*disk_read)       (BYTE*, DWORD, UINT);       /*!< Read Sector(s)                            */
#if _USE_WRITE == 1
	DRESULT (*disk_write)      (const BYTE*, DWORD, UINT); /*!< Write Sector(s) when _USE_WRITE = 0       */
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	DRESULT (*disk_ioctl)      (BYTE, void*);              /*!< I/O control operation when _USE_IOCTL = 1 */
#endif /* _USE_IOCTL == 1 */
	unsigned char*	TAG;
	unsigned char drv_num;
}ll_diskio_drv;

typedef struct{
	ll_diskio_drv	*drv[_VOLUMES];
	unsigned int            nbr;
}ff_disk_drv;
extern ff_disk_drv  disk;

int FATFS_RegisterDiskDriver(ll_diskio_drv *disk_drv);
int FATFS_UnRegisterDiskDriver(unsigned char drv_num);
int FATFS_getDrivernum(unsigned char* TAG);

#endif
