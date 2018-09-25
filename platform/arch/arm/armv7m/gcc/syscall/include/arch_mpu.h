/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ARCH_MPU_H
#define ARCH_MPU_H

int arch_app_init(struct m_app_info_t *app_info[]);

int arch_app_prepare(int app_id);

#endif // ARCH_MPU_H
