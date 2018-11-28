/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "hal/soc/soc.h"

#include "vfs_api.h"

static int32_t vfs_i2c_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_i2c_close(vfs_file_t *fp);
static int32_t vfs_i2c_read(vfs_file_t *fp, void *buf, uint32_t nbytes);
static int32_t vfs_i2c_write(vfs_file_t *fp, const void *buf, uint32_t nbytes);

/* i2c driver struct */
const vfs_file_ops_t i2c_ops =
{
    .open  = vfs_i2c_open,
    .read  = vfs_i2c_read,
    .close = vfs_i2c_close,
    .write = vfs_i2c_write
};

static int32_t vfs_i2c_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    i2c_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (i2c_dev_t *)(fp->node->i_arg);

            /*  init i2c device. */
            ret = hal_i2c_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_i2c_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    i2c_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (i2c_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_i2c_finalize(pdev);
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

static int32_t vfs_i2c_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    i2c_dev_t *pdev = NULL;          /* device pointer */

    uint16_t dev_addr = 0;     /* dev address */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (i2c_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get the device address. */
            dev_addr = pdev->config.dev_addr;

            /* get data from i2c. */
            ret = hal_i2c_master_recv(pdev, dev_addr, (uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly, the return value is set to read bytes. */
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

static int32_t vfs_i2c_write(vfs_file_t *fp, const void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    i2c_dev_t *pdev = NULL;          /* device pointer */

    uint16_t dev_addr = 0;     /* dev address */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (i2c_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get the device address. */
            dev_addr = pdev->config.dev_addr;

            /* send data from i2c. */
            ret = hal_i2c_master_send(pdev, dev_addr, (const uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

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
