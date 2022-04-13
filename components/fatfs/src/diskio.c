/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "fatfs_diskio.h"
#include "ff.h"

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
#include "sd_disk/sd_disk.h"
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
