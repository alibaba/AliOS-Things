/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_gpio.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* gpio driver struct */
const struct file_ops gpio_ops =
{
    .open = vfs_gpio_open,
    .close = vfs_gpio_close,
    .read = vfs_gpio_read,
    .ioctl = vfs_gpio_ioctl
};

int vfs_gpio_open(inode_t *inode, file_t *fp)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

            /*  init gpio device. */
            ret = hal_gpio_init(gpio_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_gpio_close(file_t *fp)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

            /* lock the device. */
            ret = aos_mutex_lock(&fp->node->mutex, AOS_WAIT_FOREVER);
            if (ret == 0) {

                /* turns off hardware. */
                ret = hal_gpio_finalize(gpio_dev);
            }

            /* unlock the device. */
            aos_mutex_unlock(&fp->node->mutex);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

ssize_t vfs_gpio_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

        /* lock the device. */
        ret = aos_mutex_lock(&fp->node->mutex, AOS_WAIT_FOREVER);
        if (ret == 0) {

            /* get data from gpio. */ 
            ret = hal_gpio_input_get(gpio_dev, (uint32_t *)buf);

            /* If the data is read correctly and the number of read data 
            bytes is not negative, the return value is set to read bytes. */
            if (ret == 0) {
                ret = sizeof(uint32_t);
            }
        }

        /* unlock the device. */
        aos_mutex_unlock(&fp->node->mutex);
    } else {
        ret = -EINVAL;
    } 
    
    return ret;
}

int vfs_gpio_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */       
    gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

    /* lock the device. */
    ret = aos_mutex_lock(&fp->node->mutex, AOS_WAIT_FOREVER);
    if (ret != 0) {
        return ret;
    }

    switch(cmd) {
        case IOCTL_GPIO_OUTPUT_HIGHT:
            ret = hal_gpio_output_high(gpio_dev);    /* output hight */
            break;
        case IOCTL_GPIO_OUTPUT_LOW:
            ret = hal_gpio_output_low(gpio_dev);     /* output low */
            break;
        case IOCTL_GPIO_OUTPUT_TOGGLE:
            ret = hal_gpio_output_toggle(gpio_dev);  /* toggle output */
            break;           
        default:
            ret = -EINVAL;
            break;
    }

    /* unlock the device. */
    aos_mutex_unlock(&fp->node->mutex);

    return ret;
}
