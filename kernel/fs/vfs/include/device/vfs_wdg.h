/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_WDG_H
#define AOS_VFS_WDG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* cmd for ioctl */
#define IOCTL_WDG_RELOAD 1 /* reload watchdog */

/* wdg driver struct */
extern const struct file_ops wdg_ops;

/**
 * This function is used to open wdg device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_wdg_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close wdg device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_wdg_close(file_t *fp);

/**
 * This function performs device input and output operations.
 *
 * @param[in]  fp   device pointer.
 * @param[in]  cmd  command of input and output operating.
 * @param[in]  arg  argument of input and output operating. 
 *
 * @return  0 on success, negative on failure with errno set appropriately.
 */
int vfs_wdg_ioctl(file_t *fp, int cmd, unsigned long arg);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_WDG_H */

