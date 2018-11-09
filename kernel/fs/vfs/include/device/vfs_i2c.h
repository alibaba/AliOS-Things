/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_I2C_H
#define AOS_VFS_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* i2c driver struct */
extern const struct file_ops i2c_ops;

/**
 * This function is used to open i2c device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_i2c_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close i2c device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_i2c_close(file_t *fp);

/**
 * This function is used to get data from i2c.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success, 
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_i2c_read(file_t *fp, void *buf, size_t nbytes);

/**
 * This function is used to send data through i2c.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return   The positive non-zero number of bytes write on success, 
 * 0 on write nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_i2c_write(file_t *fp, const void *buf, size_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_I2C_H */

