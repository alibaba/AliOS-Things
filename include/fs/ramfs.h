/**
 * @file ramfs.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef FS_RAMFS_H
#define FS_RAMFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup fs_ramfs ramfs
 * ramfs API.
 * Used for projects' inside components. Use aos api for APP.
 * @{
 */

/**
 * ramfs register
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t ramfs_register(const char *mount_path);

/**
 * ramfs unregister
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t ramfs_unregister(const char *mount_path);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

