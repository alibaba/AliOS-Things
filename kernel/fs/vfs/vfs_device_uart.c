/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "aos/hal/soc.h"

#include "vfs_api.h"

static int32_t vfs_uart_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_uart_close(vfs_file_t *fp);
static int32_t vfs_uart_read(vfs_file_t *fp, void *buf, uint32_t nbytes);
static int32_t vfs_uart_write(vfs_file_t *fp, const void *buf, uint32_t nbytes);

/* uart driver struct */
const vfs_file_ops_t uart_ops =
{
    .open  = vfs_uart_open,
    .read  = vfs_uart_read,
    .close = vfs_uart_close,
    .write = vfs_uart_write
};

static int32_t vfs_uart_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    uart_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (uart_dev_t *)(fp->node->i_arg);

            /*  init uart device. */
            ret = hal_uart_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_uart_close(vfs_file_t *fp)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    uart_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (uart_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_uart_finalize(pdev);
            } else {
                ret = VFS_ERR_INVAL;
            }
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_uart_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    uart_dev_t *pdev = NULL;          /* device pointer */

    uint32_t recv_bytes = 0;     /* number of bytes received */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (uart_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get data from uart. */
            ret = hal_uart_recv_II(pdev, buf, nbytes, &recv_bytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly the return value is set to read bytes. */
            if (ret == 0) {
                ret = recv_bytes;
            }
        } else {
            ret = VFS_ERR_INVAL;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_uart_write(vfs_file_t *fp, const void *buf, uint32_t nbytes)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    uart_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (uart_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* send data from uart. */
            ret = hal_uart_send(pdev, buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is sent successfully, set the return
            value to nbytes. */
            if (ret == 0) {
                ret = nbytes;
            }
        } else {
            ret = VFS_ERR_INVAL;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}
