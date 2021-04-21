/**
 * @file ramfs.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef FS_RAMFS_H
#define FS_RAMFS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ramfs_aos_api ramfs
 * ramfs 对外API.
 * @{
 */

/**
 * ramfs文件系统注册接口
 * @param[in] mount_path   该文件系统挂载路径
 * @return 0 表示注册成功, 负数表示失败
 *
 */
int32_t ramfs_register(const char *mount_path);

/**
 * ramfs文件系统去注册接口
 * @param[in] mount_path   该文件系统挂载路径
 * @return 0 表示去注册成功, 负数表示失败
 *
 */
int32_t ramfs_unregister(const char *mount_path);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

