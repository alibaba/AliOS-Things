/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_FAT_DISKIO_H
#define FS_FAT_DISKIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "diskio.h"

/* Definitions of physical driver number of each drive */
#define DEV_MMC 0   /* Map MMC/SD card to physical drive 0 */
#define DEV_USB 1   /* Map USB to physical drive 1 */
#define DEV_RAM 2   /* Map ramdisk to physical drive 0 */

#define MMC_MOUNTPOINT      "/sdcard"
#define MMC_PARTITION_ID    "0:"

#define USB_MOUNTPOINT      "/usb"
#define USB_PARTITION_ID    "1:"

#define RAM_MOUNTPOINT      "/ramdisk"
#define RAM_PARTITION_ID    "2:"


#ifdef __cplusplus
 }
#endif

 
#endif
