/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/timer.h"
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

#ifndef HALAPP_TIME_PORT
#define HALAPP_TIME_PORT    3   //TIMER3
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
    memset(pgpio_out,0, sizeof(gpio_dev_t));

    pgpio_out->port = HALAPP_GPIO_OUT;
    pgpio_out->config = OUTPUT_PUSH_PULL;
    pgpio_out->priv = NULL;
    hal_gpio_init(pgpio_out);

    time.port = HALAPP_TIME_PORT;
    time.config.period = 20; //ms
    time.config.reload_mode = TIMER_RELOAD_AUTO;
    time.config.cb = hal_gpio_int_fun;
    time.config.arg = pgpio_out;

    ret = hal_timer_init(&time);
    if(ret){
        return;
    }
    hal_timer_start(&time);

}

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("hal app entry here!\r\n");

    /*GPIO demo*/
    hal_gpio_app_out();
    hal_gpio_app_toggle();
    hal_gpio_app_int();

    /*timer*/
    hal_timer_app_run();

    printf("hal app end %d \r\n", count++);

    while(1) {
        aos_msleep(1000);
    };
}
