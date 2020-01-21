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

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT     8   //PA8
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN      9
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT     9
#endif

#ifndef ON_BOARD_LED02
#define ON_BOARD_LED02      23 //PB0
#endif

#ifndef PORT_TIMER5
#define PORT_TIMER5         5   //logic num, define corresponding physical port for each board
#endif

#ifndef PORT_PWM_4
#define PORT_PWM_4          3   //logic num, define corresponding physical port for each board
#endif

/*
Set gpio_out equal to gpio_in
*/
static void hal_gpio_app_out(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t gpio_out; /*GPIO8*/
    gpio_dev_t gpio_in; /*GPIO9*/

    printf("hal_gpio_app_out in\r\n");

    gpio_out.port = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv = NULL;
    hal_gpio_init(&gpio_in);

    while((cnt--) > 0){
        ret = hal_gpio_input_get(&gpio_in, &val);

        if(0 == ret) {
            if(0 == val){
                hal_gpio_output_low(&gpio_out);
            }else if( 1 == val ){
                hal_gpio_output_high(&gpio_out);
            }
        }
        aos_msleep(10);
    }

}

/*
Set gpio_out opposite to gpio_in
*/
static void hal_gpio_app_toggle(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t gpio_out; /*GPIO8*/
    gpio_dev_t gpio_in; /*GPIO9*/

    printf("hal_gpio_app_toggle in\r\n");

    gpio_out.port = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv = NULL;
    hal_gpio_init(&gpio_in);

    while((cnt--) > 0){
        ret = hal_gpio_input_get(&gpio_in, &val);

        if(0 == ret) {
            if(0 == val){
                hal_gpio_output_high(&gpio_out);
            }else if( 1 == val ){
                hal_gpio_output_low(&gpio_out);
            }
        }
        aos_msleep(10);
    }

}

static void hal_gpio_int_fun(gpio_dev_t * gpio)
{
    if(NULL == gpio) {
        return;
    }

    hal_gpio_output_toggle(gpio);
}

/* toggle gpio_out in interrupt of gpio_int */
static void hal_gpio_app_int(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t * pgpio_out = malloc(sizeof(gpio_dev_t)); /*GPIO8*/
    if(NULL == pgpio_out) {
        return;
    }

    printf("hal_gpio_app_int in\r\n");

    memset(pgpio_out,0, sizeof(gpio_dev_t));
    gpio_dev_t gpio_int; /*GPIO9*/

    pgpio_out->port = HALAPP_GPIO_OUT;
    pgpio_out->config = OUTPUT_PUSH_PULL;
    pgpio_out->priv = NULL;
    hal_gpio_init(pgpio_out);

    gpio_int.port = HALAPP_GPIO_IN;
    gpio_int.config = IRQ_MODE;
    gpio_int.priv = NULL;
    hal_gpio_enable_irq(&gpio_int, IRQ_TRIGGER_RISING_EDGE, hal_gpio_int_fun, pgpio_out);
}

/* toggle gpio_out periodically in timer handler */
static void hal_timer_app_run(void)
{
    int32_t ret;
    timer_dev_t time;
    gpio_dev_t * pgpio_out = malloc(sizeof(gpio_dev_t)); /*GPIO8*/
    if(NULL == pgpio_out) {
        return;
    }

    printf("hal_timer_app_run in\r\n");

    memset(pgpio_out,0, sizeof(gpio_dev_t));

    pgpio_out->port = ON_BOARD_LED02;
    pgpio_out->config = OUTPUT_PUSH_PULL;
    pgpio_out->priv = NULL;
    hal_gpio_init(pgpio_out);

    time.port = PORT_TIMER5;
    time.config.period = 1000*1000; //us
    time.config.reload_mode = TIMER_RELOAD_AUTO;
    time.config.cb = hal_gpio_int_fun;
    time.config.arg = pgpio_out;

    ret = hal_timer_init(&time);
    if(ret){
        return;
    }

    hal_timer_start(&time);

    aos_msleep(5000);
    hal_timer_stop(&time);
}

/* output one pwm signal */
static void hal_pwm_app_out(void)
{
    int32_t ret;
    pwm_dev_t tim = {0};

    printf("hal_pwm_app_out in\r\n");

    tim.port = PORT_PWM_4;
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

static void hal_pwm_chg_args(void)
{
    int32_t ret;
    pwm_dev_t tim = {0};

    printf("hal_pwm_chg_args in\r\n");

    pwm_config_t para = {0.25, 1000};
    tim.port = PORT_PWM_4;
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


int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("hal app entry here!\r\n");

    #if 1
    /*GPIO demo*/
    hal_gpio_app_out();
    hal_gpio_app_toggle();
    hal_gpio_app_int();

    /*timer*/
    hal_timer_app_run();

    /*pwm*/
    hal_pwm_app_out();
    hal_pwm_chg_args();
    #endif
    printf("hal app end %d \r\n", count++);

    while(1) {
        aos_msleep(1000);
    };
}
