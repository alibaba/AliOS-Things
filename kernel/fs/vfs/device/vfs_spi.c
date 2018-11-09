/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_spi.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* spi driver struct */
const struct file_ops spi_ops =
{
    .open = vfs_spi_open,
    .close = vfs_spi_close,
    .read = vfs_spi_read,
    .write = vfs_spi_write
};

int vfs_spi_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    spi_dev_t *spi_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            spi_dev = (spi_dev_t *)(fp->node->i_arg);

            /*  init spi device. */
            ret = hal_spi_init(spi_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_spi_close(file_t *fp)
{
    int ret = -1;              /* return value */
    spi_dev_t *spi_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            spi_dev = (spi_dev_t *)(fp->node->i_arg);

            if (spi_dev != NULL) {

                /* turns off hardware. */
                ret = hal_spi_finalize(spi_dev);
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

ssize_t vfs_spi_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    spi_dev_t *spi_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        spi_dev = (spi_dev_t *)(fp->node->i_arg);

        if (spi_dev != NULL) {

            /* get data from spi. */
            ret = hal_spi_recv(spi_dev, (unsigned char *)buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly and the number of read data 
            bytes is not negative, the return value is set to read bytes. */
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

ssize_t vfs_spi_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    spi_dev_t *spi_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        spi_dev = (spi_dev_t *)(fp->node->i_arg);

        if (spi_dev != NULL) {

            /* send data from spi. */
            ret = hal_spi_send(spi_dev, (const uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

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
