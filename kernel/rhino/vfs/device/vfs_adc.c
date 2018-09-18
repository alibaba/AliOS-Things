/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_adc.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

/* adc driver struct */
const struct file_ops adc_ops =
{
    .open = vfs_adc_open,
    .read = vfs_adc_read,
    .close = vfs_adc_close
};

int vfs_adc_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            adc_dev = (adc_dev_t *)(fp->node->i_arg);

            /*  init adc device. */
            ret = hal_adc_init(adc_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_adc_close(file_t *fp)
{
    int ret = -1;              /* return value */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            adc_dev = (adc_dev_t *)(fp->node->i_arg);

            if (adc_dev != NULL) {

                /* turns off an ADC hardwar. */
                ret = hal_adc_finalize(adc_dev);
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

ssize_t vfs_adc_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */       
        adc_dev = (adc_dev_t *)(fp->node->i_arg);

        if (adc_dev != NULL) {

            /* get adc sampled value. */ 
            ret = hal_adc_value_get(adc_dev, buf, HAL_WAIT_FOREVER);

            /* If the data is got successfully, set the return 
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
