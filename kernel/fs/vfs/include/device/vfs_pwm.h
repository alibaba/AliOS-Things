/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_PWM_H
#define AOS_VFS_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* cmd for ioctl */
#define IOCTL_PWM_OUTPUT_START 1 /* start output pwm */
#define IOCTL_PWM_OUTPUT_STOP  2 /* stop output pwm */

/* pwm driver struct */
extern const struct file_ops pwm_ops;

/**
 * This function is used to open pwm device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_pwm_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close pwm device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_pwm_close(file_t *fp);

/**
 * This function performs device input and output operations.
 *
 * @param[in]  fp   device pointer.
 * @param[in]  cmd  command of input and output operating.
 * @param[in]  arg  argument of input and output operating. 
 *
 * @return  0 on success, negative on failure with errno set appropriately.
 */
int vfs_pwm_ioctl(file_t *fp, int cmd, unsigned long arg);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_PWM_H */

