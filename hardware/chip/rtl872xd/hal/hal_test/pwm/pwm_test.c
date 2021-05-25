/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "aos/hal/pwm.h"
#include "pwm_test.h"

#define PWM_TESTCASE_NUM 10

pwm_dev_t    pwm_dev;
pwm_config_t config;

pwm_config_t testsace_config[PWM_TESTCASE_NUM] = {{.freq = 1000, .duty_cycle = 0.5f},
                                                  {.freq = 1000, .duty_cycle = 0.0f},
                                                  {.freq = 1000, .duty_cycle = 0.3f},
                                                  {.freq = 1000, .duty_cycle = 0.7f},
                                                  {.freq = 1000, .duty_cycle = 1.0f},
                                                  {.freq = 2000, .duty_cycle = 0.5f},
                                                  {.freq = 2000, .duty_cycle = 0.0f},
                                                  {.freq = 2000, .duty_cycle = 0.3f},
                                                  {.freq = 2000, .duty_cycle = 0.7f},
                                                  {.freq = 2000, .duty_cycle = 1.0f}};

void hal_pwm_test_init(void)
{
    int i = 0;

    printf("*********** pwm test start ! ***********\n");

    printf("enter the testcase you want to test: \n");
    printf("for example: enter \"pwm_testcase1\" to test frequence 1000hz duty_cycle 50%%\n");

    for (i = 0; i < PWM_TESTCASE_NUM; ++i) {
        printf("pwm_testcase%d [frequence:%dhz  duty_cycle:%d%% ]\n",(i + 1) ,testsace_config[i].freq, (int)(testsace_config[i].duty_cycle * 100));
    }
}

void hal_pwm_test_output(char *msg)
{
    int ret = -1;
    int num = 0;

    num = atoi(msg);

    if (num > PWM_TESTCASE_NUM) {
        printf("testcase number error !\n");
        return;
    }

    printf("pwm output frequence: %dhz duty_cycle: %d%%\n", testsace_config[num - 1].freq, (int)(testsace_config[num - 1].duty_cycle * 100));

    hal_pwm_stop(&pwm_dev);

    pwm_dev.port = PORT_PWM_TEST;
    pwm_dev.config.freq = testsace_config[num - 1].freq;
    pwm_dev.config.duty_cycle = testsace_config[num - 1].duty_cycle;
    ret = hal_pwm_init(&pwm_dev);
    if (ret != 0) {
        printf("hal_pwm_init error !\n");
        return;
    }

    hal_pwm_start(&pwm_dev);

    printf("please observe the output waveform through the oscilloscope to judge whether it is correct ！\n");
}

void hal_pwm_test_finalize(void)
{
    int ret = -1;

    ret = hal_pwm_finalize(&pwm_dev);
    if (ret != 0) {
        printf("hal_pwm_finalize error !\n");
        return;
    }

    printf("pwm test result: succeed !\n");

    printf("*********** pwm test end ! ***********\n");
}
