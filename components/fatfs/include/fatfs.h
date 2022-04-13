/**
 * @file fatfs.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef FS_FATFS_H
#define FS_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup fs_fatfs fatfs
 * fatfs API.
 * Used for projects' inside components. Use aos api for APP.
 * @{
 */

/**
 * fatfs register
 *
 * @note: switch DEV_MMC\DEV_USB\DEV_RAM to support different medium.
 * @return 0 on success, negative error on failure
 *
 */
int32_t fatfs_register(void);

/**
 * fatfs unregister
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t fatfs_unregister(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

