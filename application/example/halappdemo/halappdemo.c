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
extern void hal_pwm_app_staitc_out(void);
extern void hal_pwm_app_dynamic_out(void);
#endif

#if (AOS_HAL_ADC_ENABLED > 0)
extern void hal_adc_app_out(void);
#endif

#if (AOS_HAL_DAC_ENABLED > 0)
extern void hal_dac_app_out(void);
#endif

int application_start(int argc, char *argv[])
{
    printf("hal app test start\r\n");

#if (AOS_HAL_RTC_ENABLED > 0)
    /* init rtc and set the time */
    hal_rtc_app_init();
#endif

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
    hal_pwm_app_static_out();
    hal_pwm_app_dynamic_out();
#endif

    /* adc */
#if (AOS_HAL_ADC_ENABLED > 0)
    hal_adc_app_out();
#endif

    /* dac */
#if (AOS_HAL_DAC_ENABLED > 0)
    hal_dac_app_out();
#endif

#if (AOS_HAL_RTC_ENABLED > 0)
    /* get time and show time */
    hal_rtc_app_gettime();
#endif

#if (AOS_HAL_WDG_ENABLED > 0)
    printf("start to test watchdog.\r\n");
    printf("After 10 seconds the system will be restarted by"
            "triggering the watchdog\r\n");
    hal_watchdog_app_enable();
#endif

    printf("hal app test end!\r\n");

    while (1) {
        aos_msleep(1000);
    };
}
