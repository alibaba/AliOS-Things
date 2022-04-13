/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_PSRAM_H__
#define __HAL_PSRAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum HAL_PSRAM_ID_T {
    HAL_PSRAM_ID_0 = 0,
    HAL_PSRAM_ID_NUM,
};

void hal_psram_sleep(void);
void hal_psram_wakeup(void);
void hal_psram_init(void);
void psram_read_reg(uint32_t reg, uint32_t *val);
void hal_psram_phy_sleep();
#ifdef __cplusplus
}
#endif

#endif

