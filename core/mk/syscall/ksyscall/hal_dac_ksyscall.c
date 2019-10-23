/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "aos/hal/dac.h"
#include "hal_dac_syscall_arg.h"

int32_t sys_hal_dac_init_stub(void *arg)
{
    hal_dac_init_syscall_arg_t *_arg = arg;

    return hal_dac_init(_arg->dac);
}

int32_t sys_hal_dac_start_stub(void *arg)
{
    hal_dac_start_syscall_arg_t *_arg = arg;

    return hal_dac_start(_arg->dac, _arg->channel);
}

int32_t sys_hal_dac_stop_stub(void *arg)
{
    hal_dac_stop_syscall_arg_t *_arg = arg;

    return hal_dac_stop(_arg->dac, _arg->channel);
}

int32_t sys_hal_dac_set_value_stub(void *arg)
{
    hal_dac_set_value_syscall_arg_t *_arg = arg;

    return hal_dac_set_value(_arg->dac, _arg->channel, _arg->data);
}

int32_t sys_hal_dac_get_value_stub(void *arg)
{
    hal_dac_get_value_syscall_arg_t *_arg = arg;

    return hal_dac_get_value(_arg->dac, _arg->channel);
}

int32_t sys_hal_dac_finalize_stub(void *arg)
{
    hal_dac_finalize_syscall_arg_t *_arg = arg;

    return hal_dac_finalize(_arg->dac);
}

