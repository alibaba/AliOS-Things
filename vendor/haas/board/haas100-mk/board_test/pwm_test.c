/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "aos/hal/pwm.h"
#include "aos/cli.h"

static int32_t pwm_test_process()
{
    int32_t ret = 0;
    pwm_dev_t pwm = {0, {0.0, 25}, NULL};
    for (int k=0; k<4; k++)
    {
        pwm.port = k;
        printf("\r\n=====pwm test : testing...===\r\n");
        ret = hal_pwm_init(&pwm);
        if(ret)
        {
            printf("\r\n=====pwm test : pwm init failed===\r\n");
            return -1;
        }

        ret = hal_pwm_start(&pwm);
        if(ret)
        {
            printf("\r\n=====pwm test : pwm start failed===\r\n");
            return -1;
        }

        pwm_config_t para = {0.0, 25};
        for (int j=0; j<2; j++) {
            for (int i=0; i<100; i++) {
                para.duty_cycle = 0.01*i;
                hal_pwm_para_chg(&pwm, para);
                osDelay(50);
            }
            for (int i=10; i>0; i--) {
                para.duty_cycle = 0.01*i;
                hal_pwm_para_chg(&pwm, para);
                osDelay(50);
            }
        }
        hal_pwm_stop(&pwm);
        hal_pwm_finalize(&pwm);
    }

    printf("=====pwm test : Done=====\r\n");

    return 0;

}

static void pwm_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*************************** PWM Test **************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: PWM Test pin is 9, 13, 31, 38 in double-pins       **\r\n");
    printf("** Note: PWM Param:                                          **\r\n");
    printf("** -------- Frequency: 1KHz                                  **\r\n");
    printf("** -------- Step: 1                                          **\r\n");
    printf("** How to test: pls use oscilloscope to observe              **\r\n");
    printf("** the pwm waveform                                          **\r\n");
    printf("***************************************************************\r\n");
    printf("=====pwm test : Start=====\r\n");

    ret = pwm_test_process();
    if (ret)
    {
        printf("\r\n=====pwm test : Failed ===\r\n");

    }
}

static void handle_pwmtest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    pwm_autotest();
}

static struct cli_command pwmtest_cmd = {
    .name     = "pwmtest",
    .help     = "pwmtest",
    .function = handle_pwmtest_cmd
};

void pwm_test()
{
    aos_cli_register_command(&pwmtest_cmd);
    pwm_autotest();
}
