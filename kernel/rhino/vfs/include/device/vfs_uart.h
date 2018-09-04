/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_UART_H
#define AOS_VFS_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* uart driver struct */
extern const struct file_ops uart_ops;

/**
 * This function is used to open uart device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_uart_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close uart device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_uart_close(file_t *fp);

/**
 * This function is used to get data from uart.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success, 
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_uart_read(file_t *fp, void *buf, size_t nbytes);

/**
 * This function is used to send data through uart.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return   The positive non-zero number of bytes write on success, 
 * 0 on write nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_uart_write(file_t *fp, const void *buf, size_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_UART_H */

