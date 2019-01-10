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

#define DEV_FLASH1 3   /* Map raw flash */
#define DEV_FLASH2 4   /* Map raw flash */


#define MMC_MOUNTPOINT      "/sdcard"
#define MMC_PARTITION_ID    "0:"

#define USB_MOUNTPOINT      "/usb"
#define USB_PARTITION_ID    "1:"

#define RAM_MOUNTPOINT      "/ramdisk"
#define RAM_PARTITION_ID    "2:"

#define FLASH1_MOUNTPOINT   "/fatfs"
#define FLASH1_PARTITION_ID "3:"

#define FLASH2_MOUNTPOINT   "/fatfs2"
#define FLASH2_PARTITION_ID "4:"



#ifdef __cplusplus
 }
#endif

 
#endif
