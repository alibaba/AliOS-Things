/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_PHYIF_H__
#define __HAL_PHYIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

int hal_phyif_open(uint32_t cs);

int hal_phyif_close(uint32_t cs);

int hal_phyif_reg_write(unsigned short reg, unsigned short val);

int hal_phyif_reg_read(unsigned short reg, unsigned short *val);

#ifdef __cplusplus
}
#endif

#endif

