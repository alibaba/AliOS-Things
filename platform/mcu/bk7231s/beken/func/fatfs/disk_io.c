#include "include.h"
#include "arm_arch.h"
#include "diskio.h"

#if CFG_USE_SDCARD_HOST
#include "sdio_driver.h"
#include "sdcard.h"
#include "sdcard_pub.h"
#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"
#include "target_util_pub.h"
#include "ff.h"
#include "usb_pub.h"

#define USB_RET_OK			0
#define USB_RET_ERROR 		1
#define USB_RET_CONNECT 	2
#define USB_RET_DISCONNECT  3
#define USB_RET_READ_OK 	4
#define USB_RET_WRITE_OK 	5

extern void MUSB_Host_init(void);
extern uint8_t MGC_MsdGetMediumstatus(void);
extern uint32_t MUSB_NoneRunBackground(void);

uint8 udisk_init(void)
{
    uint32 ret = USB_RET_ERROR;

    if (MGC_MsdGetMediumstatus())
    {
        ret = USB_RET_OK;
    }
    else
    {
        MUSB_Host_init();

        while (1)
        {
            ret = MUSB_NoneRunBackground();
            printf("udisk_init: ret = 0x%lx\r\n", ret);
            if (MGC_MsdGetMediumstatus())
            {
                printf("get_HfiMedium_size = 0x%lx\r\n", get_HfiMedium_size());
                printf("get_HfiMedium_blksize = 0x%lx\r\n", get_HfiMedium_blksize());
                ret = USB_RET_OK;
                break;
            }
            else
            {
                if((USB_RET_DISCONNECT == ret) || (USB_RET_ERROR == ret))
                {
                    break;
                }
            }
        }
    }

    return ret;
}

static DD_HANDLE sdcard_hdl;
DSTATUS disk_initialize (uint8 pdrv)
{
    int cnt = 5;
    UINT32 status;

    os_printf("disk_initialize\r\n");
    if(pdrv == DISK_TYPE_SD)
    {
        while(cnt--)
        {
            sdcard_hdl = ddev_open(SDCARD_DEV_NAME, &status, 0);
            if(DD_HANDLE_UNVALID != sdcard_hdl)
            {
                return RES_OK;
            }
            FAT_WARN("SD init retry cnt = %d\r\n", cnt);
        }
    }
    else if(pdrv == DISK_TYPE_UDISK)
    {
        if(udisk_init() == USB_RET_OK)
            return RES_OK;
        return RES_OK;
    }
    return STA_NOINIT;
}


DSTATUS disk_status (uint8 pdrv)
{
    return RES_OK;
}

DRESULT disk_read (
    uint8 pdrv,
    uint8 *buff,
    uint32 start_sector,
    uint32 sector_cnt
)
{
    uint32 err;

    os_printf("disk_read: pdrv=%d, buff=0x%p, start_sector=0x%x, sector_cnt=0x%x\r\n", pdrv, buff, start_sector, sector_cnt);
    if( pdrv == DISK_TYPE_SD)
    {
        err = ddev_read(sdcard_hdl, (char *)buff, sector_cnt, start_sector);
        if(err != SD_OK )
            return RES_ERROR;
    }
    else if(pdrv == DISK_TYPE_UDISK)
    {
        err = MUSB_HfiRead(start_sector, sector_cnt, buff);
        os_printf("err = 0x%x\r\n", err);
        if(err != USB_RET_OK)
        {
            os_printf("disk_read: RES_ERROR\r\n");
            return RES_ERROR;
        }
        os_printf("disk_read: RES_OK\r\n");
        return RES_OK;
    }
    return RES_OK;
}


DSTATUS disk_close(void)
{
    os_printf("disk_close\r\n");
    ddev_close(sdcard_hdl);

    sdcard_hdl = DD_HANDLE_UNVALID;
    return RES_OK;
}

#endif


#if CFG_USE_USB_HOST
#include <stdio.h>
#include <string.h>
#include "driver_udisk.h"

enum
{
    DISK_TYPE_SD = 0,
    DISK_TYPE_UDISK
};

static uint8 cur_disk_type = DISK_TYPE_SD;

DSTATUS disk_initialize(uint8 pdrv)
{
    cur_disk_type = pdrv;
	
	os_printf("DISK_INITIALIZE\r\n");

    if (pdrv == DISK_TYPE_SD)
    {
        return RES_ERROR;
    }
    else if (udisk_init() == USB_RET_OK)
    {
        return RES_OK;
    }

    return STA_NOINIT;
}

DSTATUS disk_status(BYTE drv)
{
    return RES_OK;
}

DRESULT disk_read (
    BYTE drv,
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Sector address (LBA) */
    BYTE count		/* Number of sectors to read (1..255) */
)
{
    volatile int try_num = 0;
    cur_disk_type = drv;
	DRESULT ret;

	ret = RES_OK;
    if (drv == DISK_TYPE_SD)
    {
        ret = RES_ERROR;
    }
    else
    {
        if (udisk_rd_blk_sync(sector, count, buff) !=  USB_RET_OK)
        {
        	os_printf("disk_read_error\r\n");
            ret =  RES_ERROR;
        }
    }
	
    return ret;
}

#if (_READONLY == 0)
DRESULT disk_write (
    BYTE drv,
    const BYTE *buff,	        /* Data to be written */
    DWORD sector,		/* Sector address (LBA) */
    BYTE count			/* Number of sectors to write (1..255) */
)
{
    uint8 res = 0;
    cur_disk_type = drv;

    if (!count) return RES_PARERR;      // count2??¨¹¦Ì¨¨¨®¨²0¡ê?¡¤??¨°¡¤¦Ì??2?¨ºy¡ä¨ª?¨®

    if ( cur_disk_type == DISK_TYPE_SD)
    {
        return RES_ERROR;
    }
    else
    {
        res = udisk_wr_blk_sync((int)sector, (int)count, (uint8 *)buff);
    }

    if (res == 0x00)return RES_OK;
    else return RES_ERROR;
}
#endif

DRESULT disk_ioctl (
    BYTE drv,
    BYTE ctrl,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_OK;
    cur_disk_type = drv;
    if (cur_disk_type == DISK_TYPE_SD)
    {
        return RES_ERROR;
    }
    else
    {
        switch(ctrl)
        {
        case CTRL_SYNC:
            //    if (SD_WaitReady()==0)res = RES_OK;
            //   else res = RES_ERROR;
            //	Delay(10000);
            //		    	printf("CTRL_SYNC \r\n");
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(WORD *)buff = 8;
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = udisk_get_size();
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
            break;
        }
    }
    return res;
}

DWORD get_fattime (void)
{
    return 0;
}

uint8 Media_is_online(void)
{
    if (cur_disk_type == DISK_TYPE_SD)
        //        return SD_is_attached();
        return RES_ERROR;
    else
        return udisk_is_attached();
}

uint8 get_cur_media_type(void)
{
    return cur_disk_type;
}

DRESULT disk_unmount(uint8 pdrv)
{
    if (pdrv == DISK_TYPE_SD)
        //		SD_SPI_Uninit();
        return RES_ERROR;
    else
        //    	udisk_uninit();
        return RES_OK;
}

#endif

