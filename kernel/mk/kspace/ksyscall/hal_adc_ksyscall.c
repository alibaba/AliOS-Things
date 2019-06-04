/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_ADC_SYSCALL > 0)

#include <stdint.h>
#include "aos/hal/adc.h"
#include "hal_adc_syscall_arg.h"

int32_t sys_hal_adc_init_stub(void *arg)
{
    hal_adc_init_syscall_arg_t *_arg = arg;

    return hal_adc_init(_arg->adc);
}

int32_t sys_hal_adc_value_get_stub(void *arg)
{
    hal_adc_value_get_syscall_arg_t *_arg = arg;

    return hal_adc_value_get(_arg->adc, _arg->output, _arg->timeout);
}

int32_t sys_hal_adc_finalize_stub(void *arg)
{
    hal_adc_finalize_syscall_arg_t *_arg = arg;

    return hal_adc_finalize(_arg->adc);
}

#endif /* RHINO_CONFIG_HAL_ADC_SYSCALL */
