/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "fatfs_diskio.h"
#include "ff.h"

#define FATFS_WAIT_FOREVER 0xFFFFFFFF

#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
#include "aos/hal/sd.h"

static sd_dev_t sdmmc;
static DSTATUS  SDMMC_initialize()
{
    DSTATUS stat = STA_NOINIT;

    memset(&sdmmc, 0, sizeof(sd_dev_t));
    if (hal_sd_init(&sdmmc) == RES_OK) {
        stat &= ~STA_NOINIT;
    }

    return stat;
}

static DSTATUS SDMMC_status()
{
    hal_sd_stat status;
    if (hal_sd_stat_get(&sdmmc, &status) == 0) {
        if (status == SD_STAT_TRANSFER)
            return 0;
    }

    return STA_NOINIT;
}

static DRESULT SDMMC_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;

    if (hal_sd_blks_read(&sdmmc, buff, sector, count, FATFS_WAIT_FOREVER) != 0) {
        res = RES_ERROR;
    }

    return res;
}

static DRESULT SDMMC_write(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;

    if (hal_sd_blks_write(&sdmmc, buff, sector, count, FATFS_WAIT_FOREVER) != 0) {
        res = RES_ERROR;
    }

    return res;
}

static DRESULT SDMMC_ioctl(BYTE cmd, void *buff)
{
    DRESULT       res = RES_ERROR;
    hal_sd_info_t info;

    if (SDMMC_status() & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd) {
        case CTRL_SYNC:
            res = RES_OK;
            break;

#if FF_USE_MKFS
        case GET_SECTOR_COUNT:
            if (hal_sd_info_get(&sdmmc, &info) == 0) {
                *(DWORD *)buff = info.blk_nums;
                res            = RES_OK;
            }
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            res            = RES_OK;
            break;

        case GET_FORMAT_OPTION:
            *(BYTE *)buff = FM_ANY;
            res           = RES_OK;
            break;
#endif

        case GET_SECTOR_SIZE:
            if (hal_sd_info_get(&sdmmc, &info) == 0) {
                *(WORD *)buff = info.blk_size;
                res           = RES_OK;
            }
            break;
        default:
            res = RES_PARERR;
    }

    return res;
}

#endif

DSTATUS ff_disk_status(BYTE pdrv)
{
    switch (pdrv) {
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
        case DEV_MMC:
            return SDMMC_status();
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
        case DEV_USB:
            retutn USB_disk_status();
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
        case DEV_RAM:
            return RAM_disk_status();
#endif

        default:
            break;
    }
    return STA_NOINIT;
}

DSTATUS ff_disk_initialize(BYTE pdrv)
{
    switch (pdrv) {
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
        case DEV_MMC:
            return SDMMC_initialize();
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
        case DEV_USB:
            return USB_disk_initialize();
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
        case DEV_RAM:
            return RAM_disk_initialize();
#endif
        default:
            break;
    }
    return STA_NOINIT;
}

DRESULT ff_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    switch (pdrv) {
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
        case DEV_MMC:
            return SDMMC_read(buff, sector, count);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
        case DEV_USB:
            return USB_disk_read(buff, sector, count);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
        case DEV_RAM:
            return RAM_disk_read(buff, sector, count);
#endif

        default:
            break;
    }
    return RES_PARERR;
}

DRESULT ff_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    switch (pdrv) {
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
        case DEV_MMC:
            return SDMMC_write((BYTE *)buff, sector, count);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
        case DEV_USB:
            return USB_disk_write(buff, sector, count);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
        case DEV_RAM:
            return RAM_disk_write(buff, sector, count);
#endif

        default:
            break;
    }
    return RES_PARERR;
}

DRESULT ff_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    switch (pdrv) {
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
        case DEV_MMC:
            return SDMMC_ioctl(cmd, buff);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
        case DEV_USB:
            return USB_disk_ioctl(cmd, buff);
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
        case DEV_RAM:
            return RAM_disk_ioctl(cmd, buff);
#endif

        default:
            break;
    }
    return RES_PARERR;
}
