/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_wdg.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* wdg driver struct */
const struct file_ops wdg_ops =
{
    .open = vfs_wdg_open,
    .close = vfs_wdg_close,
    .ioctl = vfs_wdg_ioctl
};

int vfs_wdg_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    wdg_dev_t *wdg_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            wdg_dev = (wdg_dev_t *)(fp->node->i_arg);

            /*  init wdg device. */
            ret = hal_wdg_init(wdg_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_wdg_close(file_t *fp)
{
    int ret = -1;              /* return value */
    wdg_dev_t *wdg_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            wdg_dev = (wdg_dev_t *)(fp->node->i_arg);

            if (wdg_dev != NULL) {

                /* turns off hardware. */
                ret = hal_wdg_finalize(wdg_dev);
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

int vfs_wdg_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;              /* return value */
    wdg_dev_t *wdg_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */
    wdg_dev = (wdg_dev_t *)(fp->node->i_arg);

    if (wdg_dev == NULL) {
        return -EINVAL;
    }

    switch(cmd) {
        case IOCTL_WDG_RELOAD:
            hal_wdg_reload(wdg_dev);
            ret = VFS_SUCCESS;
            break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}
