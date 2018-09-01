/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_ADC_H
#define AOS_VFS_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* adc driver struct */
extern const struct file_ops adc_ops;

/**
 * This function is used to open adc device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_adc_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close adc device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_adc_close(file_t *fp);

/**
 * This function is used to complete the data sample and get the sampled value.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for sampled data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success, 
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_adc_read(file_t *fp, void *buf, size_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_ADC_H */

