/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ARCH_MPU_H
#define ARCH_MPU_H

int arch_mpu_init(void);

int arch_app_prepare(uapp_info_t *info, int pid);

#endif /* ARCH_MPU_H */
