/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/pwm.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_PWM_ENABLED > 0)

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef HALAPP_PWM_TEST
#define HALAPP_PWM_TEST          PORT_PWM_4   //logic num, define corresponding physical port for each board
#endif

/* output one pwm signal */
void hal_pwm_app_out(void)
{
    int32_t ret;
    pwm_dev_t tim = {0};

    printf("hal_pwm_app_out in\r\n");

    tim.port = HALAPP_PWM_TEST;
    tim.config.freq = 1000;
    tim.config.duty_cycle = 0.5;
    tim.priv = NULL;

    ret = hal_pwm_init(&tim);
    if(ret){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return;
    }

    hal_pwm_start(&tim);

    aos_msleep(50000);

    hal_pwm_stop(&tim);
    hal_pwm_finalize(&tim);
}

void hal_pwm_chg_args(void)
{
    int32_t ret;
    pwm_dev_t tim = {0};

    printf("hal_pwm_chg_args in\r\n");

    pwm_config_t para = {0.25, 1000};
    tim.port = HALAPP_PWM_TEST;
    tim.config.freq = 1000;
    tim.config.duty_cycle = 0.5;
    tim.priv = NULL;

    ret = hal_pwm_init(&tim);
    if(ret){
        return;
    }

    hal_pwm_start(&tim);

    aos_msleep(50000);

    hal_pwm_para_chg(&tim, para);

    aos_msleep(50000);

    hal_pwm_stop(&tim);
    hal_pwm_finalize(&tim);
}

#endif
