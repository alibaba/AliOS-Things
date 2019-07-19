/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_ADC_SYSCALL_ARG_H
#define HAL_ADC_SYSCALL_ARG_H

#include "aos/hal/adc.h"

typedef struct {
    adc_dev_t *adc;
} hal_adc_init_syscall_arg_t;

typedef struct {
    adc_dev_t *adc;
    void      *output;
    uint32_t   timeout;
} hal_adc_value_get_syscall_arg_t;

typedef struct {
    adc_dev_t *adc;
} hal_adc_finalize_syscall_arg_t;

#endif /* HAL_ADC_SYSCALL_ARG_H */

