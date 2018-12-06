/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "aos/hal/soc.h"

#include "vfs_api.h"

/* cmd for ioctl */
#define IOCTL_WDG_RELOAD 1 /* reload watchdog */

static int32_t vfs_wdg_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_wdg_close(vfs_file_t *fp);
static int32_t vfs_wdg_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg);

/* wgd driver struct */
const vfs_file_ops_t wdg_ops =
{
    .open  = vfs_wdg_open,
    .close = vfs_wdg_close,
    .ioctl = vfs_wdg_ioctl
};

static int32_t vfs_wdg_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    wdg_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (wdg_dev_t *)(fp->node->i_arg);

            /*  init wdg device. */
            ret = hal_wdg_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;

}

static int32_t vfs_wdg_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    wdg_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (wdg_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_wdg_finalize(pdev);
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

static int32_t vfs_wdg_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    wdg_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return VFS_ERR_INVAL;
    }

    /* get the device pointer. */
    pdev = (wdg_dev_t *)(fp->node->i_arg);

    if (pdev == NULL) {
        return VFS_ERR_INVAL;
    }

    switch(cmd) {
        case IOCTL_WDG_RELOAD:
            hal_wdg_reload(pdev);
            ret = VFS_OK;
            break;
        default:
            ret = VFS_ERR_INVAL;
            break;
    }

    return ret;
}
