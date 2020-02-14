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

/* output one pwm signal */
void hal_pwm_app_static_out(void)
{
    int cnt;
    int32_t ret;
    pwm_dev_t pwm = {0};

    printf(" hal_pwm_app_static_out start\r\n");

    pwm.port = HALAPP_PWM_TEST;
    pwm.config.freq = 1000;
    pwm.config.duty_cycle = 0.5;
    pwm.priv = NULL;

    ret = hal_pwm_init(&pwm);
    if(ret){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return;
    }

    hal_pwm_start(&pwm);

    aos_msleep(5000);

    hal_pwm_stop(&pwm);

    hal_pwm_finalize(&pwm);

    printf("hal_pwm_app_static_out end\r\n");
}

void hal_pwm_app_dynamic_out(void)
{
    int cnt;
    int32_t ret;
    pwm_dev_t pwm = {0};

    printf("hal_pwm_app_dynamic_out start\r\n");

    pwm.port = HALAPP_PWM_TEST;
    pwm.config.freq = 1000;
    pwm.config.duty_cycle = 0.00;
    pwm.priv = NULL;

    ret = hal_pwm_init(&pwm);
    if(ret){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return;
    }

    hal_pwm_start(&pwm);

    cnt = 10;
    while (cnt--) {
        printf("duty_cycle count up\r\n");
        pwm_config_t para = {0.000, 1000};
        for (int i = 0; i < 100; i++) {
            para.duty_cycle += 0.01;
            aos_msleep(10);
            hal_pwm_para_chg(&pwm, para);
        }

        printf("duty_cycle count down\r\n");

        para.duty_cycle = 1.0;
        para.freq = 1000;
        for (int i = 0; i < 100; i++) {
            para.duty_cycle -= 0.01;
            aos_msleep(10);
            hal_pwm_para_chg(&pwm, para);
        }
    }

    hal_pwm_stop(&pwm);

    hal_pwm_finalize(&pwm);

    printf("hal_pwm_app_dynamic_out end\r\n");
}

#endif
