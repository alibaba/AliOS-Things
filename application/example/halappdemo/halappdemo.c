/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/timer.h"
#include "aos/hal/pwm.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_GPIO_ENABLED > 0)
/*GPIO hal demo*/
extern void hal_gpio_app_out(void);
extern void hal_gpio_app_toggle(void);
extern void hal_gpio_app_int(void);
#endif

#if (AOS_HAL_TIMER_ENABLED > 0)
/*GPIO timer demo*/
extern void hal_timer_app_run(void);
#endif

#if (AOS_HAL_PWM_ENABLED > 0)
/*GPIO pwm demo*/
extern void hal_pwm_app_out(void);
extern void hal_pwm_chg_args(void);
#endif

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("hal app entry here!\r\n");

    /*gpio*/
#if (AOS_HAL_GPIO_ENABLED > 0)
    hal_gpio_app_out();
    hal_gpio_app_toggle();
    hal_gpio_app_int();
#endif

    /*timer*/
#if (AOS_HAL_TIMER_ENABLED > 0)
    hal_timer_app_run();
#endif

    /*pwm*/
#if (AOS_HAL_PWM_ENABLED > 0)
    hal_pwm_app_out();
    hal_pwm_chg_args();
#endif

    printf("hal app end %d \r\n", count++);

    while(1) {
        aos_msleep(1000);
    };
}
