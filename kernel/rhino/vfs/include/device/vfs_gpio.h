/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_GPIO_H
#define AOS_VFS_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* cmd for ioctl */
#define IOCTL_GPIO_OUTPUT_HIGHT  1  /* output hight */
#define IOCTL_GPIO_OUTPUT_LOW    2  /* output low */
#define IOCTL_GPIO_OUTPUT_TOGGLE 3  /* toggle output */

/* gpio driver struct */
extern const struct file_ops gpio_ops;

/**
 * This function is used to open gpio device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_gpio_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close gpio device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_gpio_close(file_t *fp);

/**
 * This function is used to get data from gpio.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success, 
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_gpio_read(file_t *fp, void *buf, size_t nbytes);

/**
 * This function performs device input and output operations.
 *
 * @param[in]  fp   device pointer.
 * @param[in]  cmd  command of input and output operating.
 * @param[in]  arg  argument of input and output operating. 
 *
 * @return  0 on success, negative on failure with errno set appropriately.
 */
int vfs_gpio_ioctl(file_t *fp, int cmd, unsigned long arg);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_GPIO_H */

