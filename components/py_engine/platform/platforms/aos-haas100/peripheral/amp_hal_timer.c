/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_timer.h"

int32_t amp_hal_timer_init(timer_dev_t *tim)
{
    return hal_timer_init(tim);
}

int32_t amp_hal_timer_start(timer_dev_t *tim)
{
    return hal_timer_start(tim);
}

void amp_hal_timer_stop(timer_dev_t *tim)
{
    hal_timer_stop(tim);
}

int32_t amp_hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    return hal_timer_para_chg(tim, para);
}

int32_t amp_hal_timer_finalize(timer_dev_t *tim)
{
    return hal_timer_finalize(tim);
}