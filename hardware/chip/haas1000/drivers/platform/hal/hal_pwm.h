/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "hal_cmu.h"

#define MAX_PWM_VALUE 0xFFFF

struct HAL_PWM_CFG_T {
    uint32_t freq;
    uint32_t ratio;
    bool inv;
    bool sleep_on;
};

int hal_pwm_set_ratio_max(uint32_t max);

int hal_pwm_enable(enum HAL_PWM_ID_T id, const struct HAL_PWM_CFG_T *cfg);

int hal_pwm_disable(enum HAL_PWM_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

