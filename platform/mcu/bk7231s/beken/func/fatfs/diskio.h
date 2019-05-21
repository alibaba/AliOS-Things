/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file
/-----------------------------------------------------------------------*/
#ifndef _DISKIO_H_
#define _DISKIO_H_

#include "include.h"
#include "arm_arch.h"
#include "uart_pub.h"
#include "ffconf.h"
#include "integer.h"

#define FAT_INTF_DEBUG

#ifdef FAT_INTF_DEBUG
#define FAT_PRT      os_printf
#define FAT_WARN     warning_prf
#define FAT_FATAL    fatal_prf
#else
#define FAT_PRT      null_prf
#define FAT_WARN     null_prf
#define FAT_FATAL    null_prf
#endif

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

#if CFG_USE_SDCARD_HOST
#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

    typedef uint8	DSTATUS;

    typedef enum
    {
        RES_OK = 0,		/* 0: Successful */
        RES_ERROR,		/* 1: R/W Error */
        RES_WRPRT,		/* 2: Write Protected */
        RES_NOTRDY,		/* 3: Not Ready */
        RES_PARERR		/* 4: Invalid Parameter */
    } DRESULT;

    typedef enum
    {
        DISK_TYPE_SD = 0,
        DISK_TYPE_UDISK
    } DISKTPYE;

    DSTATUS disk_initialize (uint8 pdrv);
    DSTATUS disk_status (uint8 pdrv);
    DRESULT disk_read (uint8 pdrv, uint8 *buff, uint32 start_sector, uint32 sector_cnt);

#ifdef __cplusplus
}
#endif

#endif // _DISKIO_DEFINED
#endif // CFG_USE_SDCARD_HOST


#if CFG_USE_USB_HOST
/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file
/-----------------------------------------------------------------------*/

#ifndef _DISKIO
#define _DISKIO

#define _READONLY	0	/* 1: Remove write functions */
#define _USE_IOCTL	1	/* 1: Use disk_ioctl fucntion */
#include  "ff.h"

/* Status of Disk Functions */
typedef BYTE 	DSTATUS;

typedef enum
{
    DISK_NUMBER_SPI_SD  = 0,
    DISK_NUMBER_SDIO_SD = 1,
    DISK_NUMBER_UDISK   = 2
} DISK_NUMBER;

/* Results of Disk Functions */
typedef enum
{
    RES_OK = 0,		/* 0: Successful */
    RES_ERROR,		/* 1: R/W Error */
    RES_WRPRT,		/* 2: Write Protected */
    RES_NOTRDY,		/* 3: Not Ready */
    RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */
int assign_drives (int, int);
DSTATUS disk_initialize (BYTE);
DSTATUS disk_initialize1 (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE, BYTE *, DWORD, BYTE);
#if	_READONLY == 0
DRESULT disk_write (BYTE, const BYTE *, DWORD, BYTE);
#endif
DRESULT disk_ioctl (BYTE, BYTE, void *);
DRESULT disk_unmount(uint8 drv);
uint8 Media_is_online(void);
uint8 get_cur_media_type(void);
/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */
/* Generic command (defined for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR	4	/* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

/* NAND specific ioctl command */
#define NAND_FORMAT			30	/* Create physical format */
#endif
#endif  // CFG_USE_USB_HOST

#endif // _DISKIO_H_
//eof

