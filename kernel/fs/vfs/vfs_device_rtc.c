/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>

#include "aos/hal/rtc.h"
#include "vfs_api.h"

static int32_t vfs_rtc_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_rtc_close(vfs_file_t *fp);
static int32_t vfs_rtc_read(vfs_file_t *fp, void *buf, uint32_t nbytes);
static int32_t vfs_rtc_write(vfs_file_t *fp, const void *buf, uint32_t nbytes);

/* rtc driver struct */
const vfs_file_ops_t rtc_ops =
{
    .open  = vfs_rtc_open,
    .read  = vfs_rtc_read,
    .close = vfs_rtc_close,
    .write = vfs_rtc_write
};

static int32_t vfs_rtc_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    rtc_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (rtc_dev_t *)(fp->node->i_arg);

            /*  init rtc device. */
            ret = hal_rtc_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_rtc_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    rtc_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (rtc_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_rtc_finalize(pdev);
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

static int32_t vfs_rtc_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    rtc_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL) && (nbytes >= sizeof(rtc_time_t))) {

        /* get the device pointer. */
        pdev = (rtc_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* set rtc time. */
            ret = hal_rtc_get_time(pdev, (rtc_time_t *)buf);

            /* If the time is set successfully, set the return
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

static int32_t vfs_rtc_write(vfs_file_t *fp, const void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    rtc_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL) && (nbytes >= sizeof(rtc_time_t))) {

        /* get the device pointer. */
        pdev = (rtc_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* set rtc time. */
            ret = hal_rtc_set_time(pdev, (const rtc_time_t *)buf);

            /* If the time is set successfully, set the return
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
