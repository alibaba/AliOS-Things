/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "ql_pwm.h"
#include "amp_hal_pwm.h"


#define MOD_STR		"PWM"

int32_t amp_hal_pwm_init(pwm_dev_t *pwm)
{
    int32_t ret = -1;
    unsigned short high_time;
    unsigned short cycle_time;

    if (pwm == NULL) {
        amp_error(MOD_STR, "pwm is null  \n");
        return ret;
    }

    if (pwm->config.freq > 1000) {
        cycle_time = 1;
    } else {
        cycle_time = 1000/pwm->config.freq;
    }

    high_time = (unsigned short)(unsigned int)(pwm->config.duty_cycle * (float)cycle_time);

    ret = ql_pwm_config(pwm->port, high_time, cycle_time);
    if (ret != 0) {
        amp_error(MOD_STR, "pwm config failed! \n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_pwm_start(pwm_dev_t *pwm)
{
    int32_t ret = -1;

    if (pwm == NULL) {
        amp_error(MOD_STR, "pwm is null  \n");
        return ret;
    }

    ret = ql_pwm_enable(pwm->port);
    if (ret != 0) {
        amp_error(MOD_STR, "pwm enable failed! \n");
        return ret;
    }
    return ret;
}

int32_t amp_hal_pwm_stop(pwm_dev_t *pwm)
{
    int32_t ret = -1;

    if (pwm == NULL) {
        amp_error(MOD_STR, "pwm is null  \n");
        return ret;
    }

    ret = ql_pwm_disable(pwm->port);
    if (ret != 0) {
        amp_error(MOD_STR, "pwm disable failed! \n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    return 0;
}

int32_t amp_hal_pwm_finalize(pwm_dev_t *pwm)
{
    return 0;
}



