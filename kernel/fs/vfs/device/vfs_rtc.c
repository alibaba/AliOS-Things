/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_rtc.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* rtc driver struct */
const struct file_ops rtc_ops =
{
    .open = vfs_rtc_open,
    .close = vfs_rtc_close,
    .read = vfs_rtc_read,
    .write = vfs_rtc_write
};

int vfs_rtc_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    rtc_dev_t *rtc_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            rtc_dev = (rtc_dev_t *)(fp->node->i_arg);

            /*  init rtc device. */
            ret = hal_rtc_init(rtc_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_rtc_close(file_t *fp)
{
    int ret = -1;              /* return value */
    rtc_dev_t *rtc_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            rtc_dev = (rtc_dev_t *)(fp->node->i_arg);  

            if (rtc_dev != NULL) {
                
                /* turns off hardware. */
                ret = hal_rtc_finalize(rtc_dev);
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

ssize_t vfs_rtc_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    rtc_dev_t *rtc_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL) && (nbytes >= sizeof(rtc_time_t))) {

        /* get the device pointer. */
        rtc_dev = (rtc_dev_t *)(fp->node->i_arg);

        if (rtc_dev != NULL) {

            /* get data from rtc. */
            ret = hal_rtc_get_time(rtc_dev, (rtc_time_t *)buf);

            /* If the data is read correctly the return value is set to nbytes. */
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

ssize_t vfs_rtc_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    rtc_dev_t *rtc_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL) && (nbytes >= sizeof(rtc_time_t))) {

        /* get the device pointer. */
        rtc_dev = (rtc_dev_t *)(fp->node->i_arg);

        if (rtc_dev != NULL) {

            /* set rtc time. */
            ret = hal_rtc_set_time(rtc_dev, (const rtc_time_t *)buf);

            /* If the time is set successfully, set the return 
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
