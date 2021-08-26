/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_SLEEP_MCU_PD_H__
#define __HAL_SLEEP_MCU_PD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

void hal_sleep_mcu_power_down(void);

void hal_sleep_mcu_power_up(void);

#ifdef __cplusplus
}
#endif

#endif

