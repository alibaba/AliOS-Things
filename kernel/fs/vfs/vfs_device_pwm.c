/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "hal/soc/soc.h"

#include "vfs_api.h"

/* cmd for ioctl */
#define IOCTL_PWM_OUTPUT_START 1 /* start output pwm */
#define IOCTL_PWM_OUTPUT_STOP  2 /* stop output pwm */

static int32_t vfs_pwm_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_pwm_close(vfs_file_t *fp);
static int32_t vfs_pwm_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg);

/* pwm driver struct */
const vfs_file_ops_t pwm_ops =
{
    .open  = vfs_pwm_open,
    .close = vfs_pwm_close,
    .ioctl = vfs_pwm_ioctl
};

static int32_t vfs_pwm_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    pwm_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (pwm_dev_t *)(fp->node->i_arg);

            /*  init pwm device. */
            ret = hal_pwm_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_pwm_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    pwm_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (pwm_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_pwm_finalize(pdev);
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

static int32_t vfs_pwm_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    pwm_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return VFS_ERR_INVAL;
    }

    /* get the device pointer. */
    pdev = (pwm_dev_t *)(fp->node->i_arg);

    if (pdev == NULL) {
        return VFS_ERR_INVAL;
    }

    switch(cmd) {
        case IOCTL_PWM_OUTPUT_START:
            ret = hal_pwm_start(pdev);
            break;
        case IOCTL_PWM_OUTPUT_STOP:
            ret = hal_pwm_stop(pdev);
            break;
        default:
            ret = VFS_ERR_INVAL;
            break;
    }

    return ret;

}
