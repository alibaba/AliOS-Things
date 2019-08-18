/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_dac_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_dac_init(dac_dev_t *dac)
{
    hal_dac_init_syscall_arg_t _arg;

    _arg.dac = dac;

    return (int32_t)SYSCALL(SYS_HAL_DAC_INIT, (void*)&_arg);
}

int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel)
{
    hal_dac_start_syscall_arg_t _arg;

    _arg.dac     = dac;
    _arg.channel = channel;

    return (int32_t)SYSCALL(SYS_HAL_DAC_START, (void*)&_arg);
}

int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel)
{
    hal_dac_stop_syscall_arg_t _arg;

    _arg.dac     = dac;
    _arg.channel = channel;

    return (int32_t)SYSCALL(SYS_HAL_DAC_STOP, (void*)&_arg);
}

int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data)
{
    hal_dac_set_value_syscall_arg_t _arg;

    _arg.dac     = dac;
    _arg.channel = channel;
    _arg.data    = data;

    return (int32_t)SYSCALL(SYS_HAL_DAC_SET_VALUE, (void*)&_arg);
}

int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel)
{
    hal_dac_get_value_syscall_arg_t _arg;

    _arg.dac     = dac;
    _arg.channel = channel;

    return (int32_t)SYSCALL(SYS_HAL_DAC_GET_VALUE, (void*)&_arg);
}

int32_t hal_dac_finalize(dac_dev_t *dac)
{
    hal_dac_init_syscall_arg_t _arg;

    _arg.dac = dac;

    return (int32_t)SYSCALL(SYS_HAL_DAC_FINALIZE, (void*)&_arg);
}

