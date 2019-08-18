/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_adc_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_adc_init(adc_dev_t *adc)
{
    hal_adc_init_syscall_arg_t _arg;

    _arg.adc = adc;

    return (int32_t)SYSCALL(SYS_HAL_ADC_INIT, (void*)&_arg);
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    hal_adc_value_get_syscall_arg_t _arg;

    _arg.adc = adc;
    _arg.output = output;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_ADC_VALUE_GET, (void*)&_arg);
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    hal_adc_finalize_syscall_arg_t _arg;

    _arg.adc = adc;

    return (int32_t)SYSCALL(SYS_HAL_ADC_FINALIZE, (void*)&_arg);
}

