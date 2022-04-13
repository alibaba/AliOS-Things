/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdbool.h>
#include <ulog/ulog.h>
#include <aos/hal/pwm.h>

#ifdef AOS_USE_BEEPER
#define TAG "beeper"

static pwm_dev_t pwm = {0, {0.0, 25}, NULL};
static pwm_enable = false;

int32_t beeper_init(void)
{
    int32_t ret;

    pwm.port = 0;
    ret = hal_pwm_init(&pwm);
    if (ret) {
        LOGE(TAG, "=====hal_pwm_init failed===\n");
        return -1;
    }

    return 0;
}

int32_t beeper_start(void)
{
    int32_t ret;

    if (pwm_enable)
        return;

    beeper_init();
    ret = hal_pwm_start(&pwm);
    if (ret) {
        LOGE(TAG, "=====hal_pwm_start failed===\n");
        return -1;
    }
#if 1
    pwm_config_t para = {0.0, 25};

    for (int i = 0; i < 10; i++) {
        para.duty_cycle = 0.1 * i;
        hal_pwm_para_chg(&pwm, para);
        // osDelay(50);
    }
#endif
    pwm_enable = true;
    return 0;
}

int32_t beeper_stop(void)
{
    int32_t ret;

    if (!pwm_enable)
        return;

    ret = hal_pwm_stop(&pwm);
    if (ret) {
        LOGE(TAG, "=====hal_pwm_stop fail===\n");
        return -1;
    }
    ret = hal_pwm_finalize(&pwm);
    if (ret) {
        LOGE(TAG, "=====hal_pwm_finalize fail===\n");
        return -1;
    }
    pwm_enable = false;
    return 0;
}
#endif
