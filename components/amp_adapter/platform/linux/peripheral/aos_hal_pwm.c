/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include "aos_hal_pwm.h"

int32_t aos_hal_pwm_init(pwm_dev_t *pwm)
{

    printf ("[%s] freq = %d \r\n", __FUNCTION__, pwm->config.freq);
    printf ("[%s] duty_cycle = %d \r\n", __FUNCTION__, pwm->config.duty_cycle);

    return 0;
}

int32_t aos_hal_pwm_start(pwm_dev_t *pwm)
{

    printf ("[%s] freq = %d \r\n", __FUNCTION__, pwm->port);

    return 0;
}

int32_t aos_hal_pwm_stop(pwm_dev_t *pwm)
{
    printf ("[%s] freq = %d \r\n", __FUNCTION__, pwm->port);

    return 0;
}

int32_t aos_hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    printf ("[%s] freq = %d \r\n", __FUNCTION__, para.freq);
    printf ("[%s] duty_cycle = %d \r\n", __FUNCTION__, para.duty_cycle);

    return 0;
}

int32_t aos_hal_pwm_finalize(pwm_dev_t *pwm)
{
 printf ("[%s]\r\n", __FUNCTION__);
 return 0;
}



