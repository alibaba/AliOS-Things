/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "aos/hal/soc.h"

#include "vfs_api.h"

static int32_t vfs_adc_open(vfs_inode_t *node, vfs_file_t *fp);
static int32_t vfs_adc_close(vfs_file_t *fp);
static int32_t vfs_adc_read(vfs_file_t *fp, void *buf, uint32_t nbytes);

/* adc driver struct */
const vfs_file_ops_t adc_ops =
{
    .open  = vfs_adc_open,
    .read  = vfs_adc_read,
    .close = vfs_adc_close
};

static int32_t vfs_adc_open(vfs_inode_t *node, vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS; /* return value */
    adc_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened */
        if (fp->node->refs == 1) {

            /* get the device pointer */
            pdev = (adc_dev_t *)(fp->node->i_arg);

            /* init adc device */
            ret = hal_adc_init(pdev);
        } else {
            ret = VFS_OK;
        }
    } else {
        ret = VFS_ERR_INVAL;
    }

    return ret;
}

static int32_t vfs_adc_close(vfs_file_t *fp)
{
    int32_t    ret  = VFS_ERR_NOSYS;  /* return value */
    adc_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pdev = (adc_dev_t *)(fp->node->i_arg);

            if (pdev != NULL) {

                /* turns off an ADC hardwar. */
                ret = hal_adc_finalize(pdev);
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

static int32_t vfs_adc_read(vfs_file_t *fp, void *buf, uint32_t nbytes)
{
    int32_t    ret  = VFS_ERR_NOSYS;  /* return value */
    adc_dev_t *pdev = NULL;          /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        pdev = (adc_dev_t *)(fp->node->i_arg);

        if (pdev != NULL) {

            /* get adc sampled value. */
            ret = hal_adc_value_get(pdev, buf, HAL_WAIT_FOREVER);

            /* If the data is got successfully, set the return
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

