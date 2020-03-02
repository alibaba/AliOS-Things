/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <stddef.h>

#include "aos/hal/timer.h"
#include "aos/hal/pwm.h"
#include "pwm.h"

#define PWM_CHANNEL_ESP8266     0
#define PWM_CHANNEL_NUM_ESP8266 1

static uint32 g_pwm_pins_map[PWM_CHANNEL_NUM_ESP8266][3] =
{
    {PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5,5}  // addr,func,pinno
}

int32_t hal_pwm_init(pwm_dev_t *tim)
{
    uint32 period;
    uint32 i;
    uint32 pduty[PWM_CHANNEL_NUM_ESP8266];
    uint32 pwm_channel_num = PWM_CHANNEL_NUM_ESP8266;

    period = 1*1000*1000/(tim->config.freq);  //us
    for(i = 0; i < PWM_CHANNEL_NUM_ESP8266; i++){
        pduty[i] = (tim->config.duty_cycle) * period;  // HIGH LEVEL us
    }

    pwm_init(period, pduty, PWM_CHANNEL_NUM_ESP8266, g_pwm_pins_map);
    return 0;
}

int32_t hal_pwm_start(pwm_dev_t *tim)
{
    (void)tim;
    pwm_start();
    return 0;
}

int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    uint32 period;
    uint32 duty;
    period = 1*1000*1000/(para.freq);  //us
    duty = (para.duty_cycle) * period;  // HIGH LEVEL us

    pwm_set_period(period);
    pwm_set_duty(duty,PWM_CHANNEL_ESP8266);

    return 0;
}

int32_t hal_pwm_stop(pwm_dev_t *tim)
{
    return -1;
}

int32_t hal_pwm_finalize(pwm_dev_t *tim)
{
    return -1;
}

