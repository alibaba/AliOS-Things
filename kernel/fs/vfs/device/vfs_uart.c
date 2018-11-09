/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_uart.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* uart driver struct */
const struct file_ops uart_ops =
{
    .open = vfs_uart_open,
    .close = vfs_uart_close,
    .read = vfs_uart_read,
    .write = vfs_uart_write
};

int vfs_uart_open(inode_t *inode, file_t *fp)
{
    int ret = -1;                /* return value */
    uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            uart_dev = (uart_dev_t *)(fp->node->i_arg);

            /*  init uart device. */
            ret = hal_uart_init(uart_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_uart_close(file_t *fp)
{
    int ret = -1;                /* return value */
    uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            uart_dev = (uart_dev_t *)(fp->node->i_arg);

            if (uart_dev != NULL) {

                /* turns off hardware. */
                ret = hal_uart_finalize(uart_dev);
            } else {
                ret = -EINVAL;
            }
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

ssize_t vfs_uart_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    uart_dev_t *uart_dev = NULL; /* device pointer */
    uint32_t recv_bytes = 0;     /* number of bytes received */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        uart_dev = (uart_dev_t *)(fp->node->i_arg);

        if (uart_dev != NULL) {

            /* get data from uart. */
            ret = hal_uart_recv_II(uart_dev, buf, nbytes, &recv_bytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly the return value is set to read bytes. */ 
            if (ret == 0) {
                ret = recv_bytes;
            }
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    } 
    
    return ret;
}

ssize_t vfs_uart_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        uart_dev = (uart_dev_t *)(fp->node->i_arg);

        if (uart_dev != NULL) {

            /* send data from uart. */ 
            ret = hal_uart_send(uart_dev, buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is sent successfully, set the return 
            value to nbytes. */
            if (ret == 0) {
                ret = nbytes;
            }
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}
