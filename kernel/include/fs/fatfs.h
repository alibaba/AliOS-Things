/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_FATFS_H
#define FS_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions of physical driver number of each drive */
#define DEV_MMC 0   /* Map MMC/SD card to physical drive 0 */
#define DEV_USB 1   /* Map USB to physical drive 1 */
#define DEV_RAM 2   /* Map ramdisk to physical drive 0 */

int32_t fatfs_register(void);

int32_t fatfs_unregister(void);

int32_t fatfs_dev_register(int32_t pdrv, const char *path);

int32_t fatfs_dev_unregister(int32_t pdrv);

int32_t fatfs_list(void);

int32_t fatfs_root2dev(const char *path);

#ifdef __cplusplus
}
#endif

#endif

