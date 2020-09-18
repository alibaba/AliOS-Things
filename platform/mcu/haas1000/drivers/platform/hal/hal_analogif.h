/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_ANALOGIF_H__
#define __HAL_ANALOGIF_H__

#ifdef __cplusplus
extern "C" {
#endif

int hal_analogif_open(void);

int hal_analogif_reg_write(unsigned short reg, unsigned short val);

int hal_analogif_reg_read(unsigned short reg, unsigned short *val);

#ifdef __cplusplus
}
#endif

#endif

