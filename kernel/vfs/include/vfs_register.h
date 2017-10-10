/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_DRIVER_H
#define VFS_DRIVER_H

#include <vfs_inode.h>

#ifdef __cplusplus
extern "C" {
#endif

int aos_register_driver(const char *path, file_ops_t *fops, void *arg);
int aos_unregister_driver(const char *path);

int aos_register_fs(const char *path, fs_ops_t *fops, void *arg);
int aos_unregister_fs(const char *path);

#ifdef __cplusplus
}
#endif

#endif    /*VFS_DRIVER_H*/

