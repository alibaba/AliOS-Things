/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_i2c.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* i2c driver struct */
const struct file_ops i2c_ops =
{
    .open = vfs_i2c_open,
    .close = vfs_i2c_close,
    .read = vfs_i2c_read,
    .write = vfs_i2c_write
};

int vfs_i2c_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    i2c_dev_t *i2c_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            i2c_dev = (i2c_dev_t *)(fp->node->i_arg);

            /*  init i2c device. */
            ret = hal_i2c_init(i2c_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_i2c_close(file_t *fp)
{
    int ret = -1;              /* return value */
    i2c_dev_t *i2c_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            i2c_dev = (i2c_dev_t *)(fp->node->i_arg);

            if (i2c_dev != NULL) {

                /* turns off hardware. */
                ret = hal_i2c_finalize(i2c_dev);
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

ssize_t vfs_i2c_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    i2c_dev_t *i2c_dev = NULL; /* device pointer */
    uint16_t dev_addr = 0;     /* dev address */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        i2c_dev = (i2c_dev_t *)(fp->node->i_arg);

        /* get the device address. */
        dev_addr = i2c_dev->config.dev_addr;

        if (dev_addr != NULL) {

            /* get data from i2c. */
            ret = hal_i2c_master_recv(i2c_dev, dev_addr, (uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

            /* If the data is read correctly, the return 
            value is set to read bytes. */ 
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

ssize_t vfs_i2c_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    i2c_dev_t *i2c_dev = NULL; /* device pointer */
    uint16_t dev_addr = 0;     /* dev address */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */       
        i2c_dev = (i2c_dev_t *)(fp->node->i_arg);

        /* get the device address. */
        dev_addr = i2c_dev->config.dev_addr;

        if (dev_addr != NULL) {

            /* send data from i2c. */ 
            ret = hal_i2c_master_send(i2c_dev, dev_addr, (const uint8_t *)buf, nbytes, HAL_WAIT_FOREVER);

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
