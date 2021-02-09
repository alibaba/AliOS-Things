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
    for (int k=0; k<1; k++)
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
            for (int i=0; i<10; i++) {
                para.duty_cycle = 0.1*i;
                hal_pwm_para_chg(&pwm, para);
                osDelay(100);
            }
            for (int i=10; i>0; i--) {
                para.duty_cycle = 0.01*i;
                hal_pwm_para_chg(&pwm, para);
                osDelay(100);
            }
        }
        hal_pwm_stop(&pwm);
        hal_pwm_finalize(&pwm);
    }
    printf("=====pwm test : Done=====\r\n");

    return 0;

}

static int pwm_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*************************** PWM Test **************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: use PWM pin as GPIO pin in Haas101:                 **\r\n");
    printf("** Test pin is (pwm0(gpio0_26), pwm1(gpio0_27))              **\r\n");
    printf("**   (qpwm2(gpio0_24), pwm3(gpio0_25)) in double-pins        **\r\n");
    printf("***************************************************************\r\n");
    printf("=====pwm test : Start=====\r\n");

    ret = pwm_test_process();
    if (ret)
    {
        printf("\r\n=====pwm test : Failed ===\r\n");
        goto fail;
    }

    printf("===Result : pwm test PASS !!! ===\r\n");
    return 0;

fail:
    printf("===Warning: pwm test FAIL !!! ===\r\n");
    return -1;
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

int pwm_test()
{
    aos_cli_register_command(&pwmtest_cmd);
    return pwm_autotest();
}
