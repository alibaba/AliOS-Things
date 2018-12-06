/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "aos/hal/soc.h"

#include "vfs_api.h"

static int32_t vfs_spi_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_spi_close(vfs_file_t *fp);
static int32_t vfs_spi_read(vfs_file_t *fp, void *buf, uint32_t nbytes);
static int32_t vfs_spi_write(vfs_file_t *fp, const void *buf, uint32_t nbytes);

/* spi driver struct */
const vfs_file_ops_t spi_ops =
{
    .open  = vfs_spi_open,
    .read  = vfs_spi_read,
    .close = vfs_spi_close,
    .write = vfs_spi_write
};

static int32_t vfs_spi_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    spi_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (spi_dev_t *)(fp->node->i_arg);

            /*  init spi device. */
            ret = hal_spi_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_spi_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    spi_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (spi_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_spi_finalize(pdev);
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

static int32_t vfs_spi_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    spi_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (spi_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get data from spi. */
            ret = hal_spi_recv(pdev, (unsigned char *)buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly and the number of read data
            bytes is not negative, the return value is set to read bytes. */
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

static int32_t vfs_spi_write(vfs_file_t *fp, const void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    spi_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (spi_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* send data from spi. */
            ret = hal_spi_send(pdev, (const uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

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
