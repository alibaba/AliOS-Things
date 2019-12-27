/**
 * @file vfs_conf.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef VFS_CONF_H
#define VFS_CONF_H

/**
 * @addtogroup vfs_conf vfsconf
 * vfs conf macro.
 *
 * @{
 */

/* vfs fs and device node all number limit */
#ifndef VFS_CONFIG_DEVICE_NODES
#define VFS_DEVICE_NODES 25
#else
#define VFS_DEVICE_NODES VFS_CONFIG_DEVICE_NODES
#endif

/* fd index base offset, ahead of which are not used by vfs */
#ifndef VFS_CONFIG_FD_OFFSET
#define VFS_FD_OFFSET 512
#else
#define VFS_FD_OFFSET VFS_CONFIG_FD_OFFSET
#endif

/* max len for file or dir name length */
#ifndef VFS_CONFIG_PATH_MAX
#define VFS_PATH_MAX 256
#else
#define VFS_PATH_MAX VFS_CONFIG_PATH_MAX
#endif

/* maximun vfs file num */
#ifndef VFS_CONFIG_MAX_FILE_NUM
#define VFS_MAX_FILE_NUM (VFS_DEVICE_NODES * 2)
#else
#define VFS_MAX_FILE_NUM VFS_CONFIG_MAX_FILE_NUM
#endif

/* whether support getting current directory or not */
#ifndef VFS_CONFIG_CURRENT_DIRECTORY_ENABLE
#define CURRENT_WORKING_DIRECTORY_ENABLE 0
#else
#define CURRENT_WORKING_DIRECTORY_ENABLE VFS_CONFIG_CURRENT_DIRECTORY_ENABLE
#endif

/** @} */

#endif /* VFS_CONF_H */

