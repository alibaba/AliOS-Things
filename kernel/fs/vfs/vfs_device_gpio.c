/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>

#include "aos/hal/gpio.h"
#include "vfs_api.h"

/* cmd for ioctl */
#define IOCTL_GPIO_OUTPUT_HIGHT  1  /* output hight */
#define IOCTL_GPIO_OUTPUT_LOW    2  /* output low */
#define IOCTL_GPIO_OUTPUT_TOGGLE 3  /* toggle output */

static int32_t vfs_gpio_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_gpio_close(vfs_file_t *fp);
static int32_t vfs_gpio_read(vfs_file_t *fp, void *buf, uint32_t nbytes);
static int32_t vfs_gpio_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg);

/* gpio driver struct */
const vfs_file_ops_t gpio_ops =
{
    .open  = vfs_gpio_open,
    .read  = vfs_gpio_read,
    .close = vfs_gpio_close,
    .ioctl = vfs_gpio_ioctl
};

static int32_t vfs_gpio_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    gpio_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (gpio_dev_t *)(fp->node->i_arg);

            /*  init gpio device. */
            ret = hal_gpio_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_gpio_close(vfs_file_t *fp)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    gpio_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (gpio_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off hardware. */
                ret = hal_gpio_finalize(pdev);
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

static int32_t vfs_gpio_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    gpio_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (gpio_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get data from gpio. */
            ret = hal_gpio_input_get(pdev, (uint32_t *)buf);

            /* If the data is read correctly and the number of read data
            bytes is not negative, the return value is set to read bytes. */
            if (ret == 0) {
                ret = sizeof(uint32_t);
            }
        } else {
            ret = VFS_ERR_INVAL;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_gpio_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    int32_t     ret  = VFS_ERR_NOSYS; /* return value */
    gpio_dev_t *pdev = NULL;          /* device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return VFS_ERR_INVAL;
    }

    /* get the device pointer. */
    pdev = (gpio_dev_t *)(fp->node->i_arg);

    if (pdev == NULL) {
        return VFS_ERR_INVAL;
    }

    switch(cmd) {
        case IOCTL_GPIO_OUTPUT_HIGHT:
            ret = hal_gpio_output_high(pdev);    /* output hight */
            break;
        case IOCTL_GPIO_OUTPUT_LOW:
            ret = hal_gpio_output_low(pdev);     /* output low */
            break;
        case IOCTL_GPIO_OUTPUT_TOGGLE:
            ret = hal_gpio_output_toggle(pdev);  /* toggle output */
            break;
        default:
            ret = VFS_ERR_INVAL;
            break;
    }

    return ret;
}


